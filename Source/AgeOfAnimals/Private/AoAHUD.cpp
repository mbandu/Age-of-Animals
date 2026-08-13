#include "EngineUtils.h"
#include "AoAHUD.h"
#include "AoAPlayerController.h"
#include "AoAPlayerState.h"
#include "AoAGameInstance.h"
#include "AoAGameState.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAEmpireData.h"
#include "AoAUnitData.h"
#include "AoABuildingData.h"
#include "Engine/Texture2D.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"

AAoAHUD::AAoAHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAoAHUD::DrawHUD()
{
	Super::DrawHUD();

	PC = Cast<AAoAPlayerController>(PlayerOwner);
	if (!PC) return;
	PS = Cast<AAoAPlayerState>(PC->PlayerState);
	GI = Cast<UAoAGameInstance>(GetGameInstance());

	if (PC->bIsDragging)
		DrawSelectionBox();

	DrawResourceBar();
	DrawSelectionPanel();
	DrawBuildPanel();
	DrawBuildingPanel();
	DrawMinimap();
	DrawHelpOverlay();

	// Check for game over
	if (auto* GS = Cast<AAoAGameState>(GetWorld()->GetGameState()))
	{
		if (GS->Phase == EGamePhase::Ended)
			DrawGameOver();
	}
}

void AAoAHUD::DrawSelectionBox()
{
	if (!Canvas) return;
	FVector2D Min(FMath::Min(PC->DragStart.X, PC->DragEnd.X), FMath::Min(PC->DragStart.Y, PC->DragEnd.Y));
	FVector2D Max(FMath::Max(PC->DragStart.X, PC->DragEnd.X), FMath::Max(PC->DragStart.Y, PC->DragEnd.Y));
	FBox2D Box(Min, Max);

	// Draw semi-transparent fill
	FCanvasTileItem FillItem(
		Min,
		FVector2D(Max.X - Min.X, Max.Y - Min.Y),
		FLinearColor(0.3f, 0.8f, 0.3f, 0.15f));
	Canvas->DrawItem(FillItem);

	// Draw border
	FCanvasBoxItem BoxItem(Min, FVector2D(Max.X - Min.X, Max.Y - Min.Y));
	BoxItem.SetColor(FLinearColor(0.3f, 0.8f, 0.3f, 0.8f));
	Canvas->DrawItem(BoxItem);
}

void AAoAHUD::DrawResourceBar()
{
	if (!PS || !Canvas) return;

	float X = 10.0f, Y = 10.0f;
	float BoxW = 320.0f, BoxH = 40.0f;

	// Background
	FCanvasTileItem BG(X - 4, Y - 4, BoxW + 8, BoxH + 8, FLinearColor(0.05f, 0.06f, 0.08f, 0.85f));
	Canvas->DrawItem(BG);

	// Resources: Food, Wood, Stone, Gold, Pop
	FString ResText = FString::Printf(TEXT("Food: %d  Wood: %d  Stone: %d  Gold: %d  Pop: %d/%d"),
		PS->Food, PS->Wood, PS->Stone, PS->Gold, PS->PopulationUsed, PS->PopulationCap);
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(ResText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

	// Age indicator
	const char* AgeNames[] = {"Tribal", "Bronze", "Iron"};
	FString AgeText = FString::Printf(TEXT("Age: %s"), ANSI_TO_TCHAR(AgeNames[FMath::Clamp(PS->CurrentAge, 0, 2)]));
	FCanvasTextItem AgeItem(FVector2D(X, Y + 20), FText::FromString(AgeText),
		GEngine->GetSmallFont(), FLinearColor(0.8f, 0.7f, 0.3f, 1.0f));
	Canvas->DrawItem(AgeItem);
}

void AAoAHUD::DrawSelectionPanel()
{
	if (!PC || PC->SelectedActors.IsEmpty()) return;

	float X = 10.0f, Y = Canvas->ClipY - 120.0f;
	float BoxW = 400.0f, BoxH = 110.0f;

	FCanvasTileItem BG(X - 4, Y - 4, BoxW + 8, BoxH + 8, FLinearColor(0.05f, 0.06f, 0.08f, 0.85f));
	Canvas->DrawItem(BG);

	// Count units by type
	int32 Counts[4] = {0, 0, 0, 0};
	const char* RoleNames[] = {"Villagers", "Warriors", "Archers", "Specials"};
	for (auto* Actor : PC->SelectedActors)
	{
		if (auto* U = Cast<AAoAUnit>(Actor))
			Counts[FMath::Clamp(U->UnitRoleIndex, 0, 3)]++;
	}

	FString SelText = TEXT("Selected: ");
	for (int i = 0; i < 4; ++i)
	{
		if (Counts[i] > 0)
			SelText += FString::Printf(TEXT("%s: %d  "), ANSI_TO_TCHAR(RoleNames[i]), Counts[i]);
	}
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(SelText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

	// Show build buttons if villagers are selected
	bool bHasVillagers = Counts[0] > 0;
	if (bHasVillagers)
	{
		FString BuildText = TEXT("[1] House  [2] Barracks  [3] Tower  [4] Town Center");
		FCanvasTextItem BuildItem(FVector2D(X, Y + 25), FText::FromString(BuildText),
			GEngine->GetSmallFont(), FLinearColor(0.6f, 0.8f, 0.6f, 1.0f));
		Canvas->DrawItem(BuildItem);
	}

	// Show commands
	FString CmdText = TEXT("Right-click: Move/Attack/Gather  [S] Stop  [A] Attack  [G] Gather");
	FCanvasTextItem CmdItem(FVector2D(X, Y + 50), FText::FromString(CmdText),
		GEngine->GetSmallFont(), FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	Canvas->DrawItem(CmdItem);
}

void AAoAHUD::DrawBuildPanel()
{
	// Build panel is shown when villagers are selected
	// In a complete UMG implementation, this would be a widget
}

void AAoAHUD::DrawBuildingPanel()
{
	if (!PC || !PC->SelectedBuilding) return;
	auto* Bld = Cast<AAoABuilding>(PC->SelectedBuilding);
	if (!Bld) return;

	float X = Canvas->ClipX - 260.0f, Y = Canvas->ClipY - 120.0f;
	float BoxW = 250.0f, BoxH = 110.0f;

	FCanvasTileItem BG(X - 4, Y - 4, BoxW + 8, BoxH + 8, FLinearColor(0.05f, 0.06f, 0.08f, 0.85f));
	Canvas->DrawItem(BG);

	const char* RoleNames[] = {"Town Center", "House", "Barracks", "Tower"};
	FString BldText = FString::Printf(TEXT("%s\nHP: %d/%d%s"),
		ANSI_TO_TCHAR(RoleNames[FMath::Clamp(Bld->BuildingRoleIndex, 0, 3)]),
		(int32)Bld->CurrentHP, (int32)Bld->MaxHP,
		Bld->IsComplete() ? TEXT("") : TEXT(" (Building...)"));
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(BldText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

	// Show training queue
	if (!Bld->TrainQueue.IsEmpty())
	{
		FString QueueText = FString::Printf(TEXT("Training: %d in queue"), Bld->TrainQueue.Num());
		FCanvasTextItem QItem(FVector2D(X, Y + 40), FText::FromString(QueueText),
			GEngine->GetSmallFont(), FLinearColor(0.8f, 0.8f, 0.3f, 1.0f));
		Canvas->DrawItem(QItem);
	}
}

void AAoAHUD::DrawMinimap()
{
	if (!Canvas) return;

	float Size = MinimapSize;
	float X = Canvas->ClipX - Size - 20.0f;
	float Y = 20.0f;

	// Background
	FCanvasTileItem BG(X - 4, Y - 4, Size + 8, Size + 8, FLinearColor(0.02f, 0.03f, 0.04f, 0.9f));
	Canvas->DrawItem(BG);

	// Draw minimap — for each visible actor, draw a colored dot
	if (PC)
	{
		// Draw units
		for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
		{
			FVector2D MiniPos = WorldToMinimap(It->GetActorLocation());
			FLinearColor Color = FLinearColor::Red;
			if (It->GetOwnerPlayerId() == (PS ? PS->GetPlayerId() : -1))
				Color = FLinearColor::Green;
			FCanvasTileItem Dot(MiniPos, FVector2D(3, 3), Color);
			Canvas->DrawItem(Dot);
		}

		// Draw buildings
		for (TActorIterator<AAoABuilding> It(GetWorld()); It; ++It)
		{
			FVector2D MiniPos = WorldToMinimap(It->GetActorLocation());
			FLinearColor Color = FLinearColor::Red;
			if (It->GetOwnerPlayerId() == (PS ? PS->GetPlayerId() : -1))
				Color = FLinearColor::Yellow;
			FCanvasTileItem Dot(MiniPos, FVector2D(5, 5), Color);
			Canvas->DrawItem(Dot);
		}

		// Draw viewport rectangle
		FVector CamLoc = PC->GetFocalLocation();
		FVector2D ViewMini = WorldToMinimap(CamLoc);
		FCanvasBoxItem ViewBox(ViewMini - FVector2D(20, 20), FVector2D(40, 40));
		ViewBox.SetColor(FLinearColor(1, 1, 1, 0.6f));
		Canvas->DrawItem(ViewBox);
	}
}

FVector2D AAoAHUD::WorldToMinimap(const FVector& WorldLoc) const
{
	float Size = MinimapSize;
	float X = Canvas->ClipX - Size - 20.0f;
	float Y = 20.0f;

	// Map world coordinates to minimap space
	// Assuming a 64x64 tile map with 100 units per tile = 6400x6400 world
	float WorldSize = 6400.0f;
	float MX = X + (WorldLoc.X / WorldSize) * Size;
	float MY = Y + (WorldLoc.Y / WorldSize) * Size;
	return FVector2D(FMath::Clamp(MX, X, X + Size), FMath::Clamp(MY, Y, Y + Size));
}

void AAoAHUD::DrawGameOver()
{
	if (!Canvas) return;

	// Dark overlay
	FCanvasTileItem Overlay(0, 0, Canvas->ClipX, Canvas->ClipY, FLinearColor(0, 0, 0, 0.6f));
	Canvas->DrawItem(Overlay);

	auto* GS = Cast<AAoAGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	FString ResultText;
	if (GS->WinnerPlayerId == -1)
		ResultText = TEXT("Draw!");
	else if (PS && GS->WinnerPlayerId == PS->GetPlayerId())
		ResultText = TEXT("Victory!");
	else
		ResultText = TEXT("Defeat");

	FCanvasTextItem TextItem(
		FVector2D(Canvas->ClipX * 0.5f - 100, Canvas->ClipY * 0.5f - 30),
		FText::FromString(ResultText),
		GEngine->GetLargeFont(), FLinearColor(1.0f, 0.9f, 0.4f, 1.0f));
	Canvas->DrawItem(TextItem);

	FString SubText = TEXT("Press ENTER for menu  |  ESC to quit");
	FCanvasTextItem SubItem(
		FVector2D(Canvas->ClipX * 0.5f - 120, Canvas->ClipY * 0.5f + 20),
		FText::FromString(SubText),
		GEngine->GetMediumFont(), FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
	Canvas->DrawItem(SubItem);
}

void AAoAHUD::DrawHelpOverlay()
{
	// Show brief control hints in the corner
	float X = 10.0f, Y = Canvas->ClipY - 30.0f;
	FString Help = TEXT("Drag: Select  |  RClick: Command  |  A: All Army  |  V: Villagers  |  S: Stop  |  H: Help");
	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Help),
		GEngine->GetSmallFont(), FLinearColor(0.5f, 0.5f, 0.5f, 0.7f));
	Canvas->DrawItem(Item);
}

void AAoAHUD::InitializeHUD()
{
	if (HUDWidgetClass)
	{
		CurrentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentHUDWidget)
			CurrentHUDWidget->AddToViewport();
	}
}

void AAoAHUD::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;
	CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	if (CurrentMenuWidget)
		CurrentMenuWidget->AddToViewport();
}

void AAoAHUD::RemoveMenuWidgets()
{
	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr;
	}
}

void AAoAHUD::DrawMinimapToTexture()
{
	// In a complete build, render the terrain to a minimap texture
	// using a render target or pixel manipulation
}

