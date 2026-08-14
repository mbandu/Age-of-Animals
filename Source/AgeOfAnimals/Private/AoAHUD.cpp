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
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "EngineUtils.h"

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
	DrawBuildingPanel();
	DrawMinimap();
	DrawHelpOverlay();

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

	// Draw semi-transparent fill
	FCanvasTileItem FillItem(Min, Max - Min, FLinearColor(0.3f, 0.8f, 0.3f, 0.15f));
	Canvas->DrawItem(FillItem);

	// Draw border
	FCanvasBoxItem BoxItem(Min, Max - Min);
	BoxItem.SetColor(FLinearColor(0.3f, 0.8f, 0.3f, 0.8f));
	Canvas->DrawItem(BoxItem);
}

void AAoAHUD::DrawResourceBar()
{
	if (!PS || !Canvas) return;

	float X = 10.0f, Y = 10.0f;

	FString ResText = FString::Printf(TEXT("Food: %d  Wood: %d  Stone: %d  Gold: %d  Pop: %d/%d"),
		PS->Food, PS->Wood, PS->Stone, PS->Gold, PS->PopulationUsed, PS->PopulationCap);
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(ResText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

	const TCHAR* AgeNames[] = {TEXT("Tribal"), TEXT("Bronze"), TEXT("Iron")};
	FString AgeText = FString::Printf(TEXT("Age: %s"), AgeNames[FMath::Clamp(PS->CurrentAge, 0, 2)]);
	FCanvasTextItem AgeItem(FVector2D(X, Y + 20), FText::FromString(AgeText),
		GEngine->GetSmallFont(), FLinearColor(0.8f, 0.7f, 0.3f, 1.0f));
	Canvas->DrawItem(AgeItem);
}

void AAoAHUD::DrawSelectionPanel()
{
	if (!PC || PC->SelectedActors.IsEmpty() || !Canvas) return;

	float X = 10.0f, Y = Canvas->ClipY - 120.0f;

	int32 Counts[4] = {0, 0, 0, 0};
	const TCHAR* RoleNames[] = {TEXT("Villagers"), TEXT("Warriors"), TEXT("Archers"), TEXT("Specials")};
	for (AActor* Actor : PC->SelectedActors)
	{
		if (auto* U = Cast<AAoAUnit>(Actor))
			Counts[FMath::Clamp(U->UnitRoleIndex, 0, 3)]++;
	}

	FString SelText = TEXT("Selected: ");
	for (int i = 0; i < 4; ++i)
	{
		if (Counts[i] > 0)
			SelText += FString::Printf(TEXT("%s: %d  "), RoleNames[i], Counts[i]);
	}
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(SelText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

	if (Counts[0] > 0)
	{
		FString BuildText = TEXT("[1] House  [2] Barracks  [3] Tower  [4] Town Center");
		FCanvasTextItem BuildItem(FVector2D(X, Y + 25), FText::FromString(BuildText),
			GEngine->GetSmallFont(), FLinearColor(0.6f, 0.8f, 0.6f, 1.0f));
		Canvas->DrawItem(BuildItem);
	}

	FString CmdText = TEXT("Right-click: Move/Attack/Gather  [S] Stop  [A] All Army  [V] Villagers");
	FCanvasTextItem CmdItem(FVector2D(X, Y + 50), FText::FromString(CmdText),
		GEngine->GetSmallFont(), FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	Canvas->DrawItem(CmdItem);
}

void AAoAHUD::DrawBuildPanel()
{
}

void AAoAHUD::DrawBuildingPanel()
{
	if (!PC || !PC->SelectedBuilding || !Canvas) return;
	auto* Bld = Cast<AAoABuilding>(PC->SelectedBuilding);
	if (!Bld) return;

	float X = Canvas->ClipX - 260.0f, Y = Canvas->ClipY - 120.0f;

	const TCHAR* RoleNames[] = {TEXT("Town Center"), TEXT("House"), TEXT("Barracks"), TEXT("Tower")};
	FString BldText = FString::Printf(TEXT("%s  HP: %d/%d%s"),
		RoleNames[FMath::Clamp(Bld->BuildingRoleIndex, 0, 3)],
		(int32)Bld->CurrentHP, (int32)Bld->MaxHP,
		Bld->IsComplete() ? TEXT("") : TEXT(" (Building...)"));
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(BldText),
		GEngine->GetSmallFont(), FLinearColor::White);
	Canvas->DrawItem(TextItem);

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
	FCanvasTileItem BG(FVector2D(X - 4, Y - 4), FVector2D(Size + 8, Size + 8), FLinearColor(0.02f, 0.03f, 0.04f, 0.9f));
	Canvas->DrawItem(BG);

	if (!PC) return;

	// Draw units as colored dots
	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		FVector2D MiniPos = WorldToMinimap(It->GetActorLocation());
		FLinearColor Color = FLinearColor::Red;
		if (PS && It->GetOwnerPlayerId() == PS->GetPlayerId())
			Color = FLinearColor::Green;
		FCanvasTileItem Dot(MiniPos, FVector2D(3, 3), Color);
		Canvas->DrawItem(Dot);
	}

	// Draw buildings
	for (TActorIterator<AAoABuilding> It(GetWorld()); It; ++It)
	{
		FVector2D MiniPos = WorldToMinimap(It->GetActorLocation());
		FLinearColor Color = FLinearColor::Red;
		if (PS && It->GetOwnerPlayerId() == PS->GetPlayerId())
			Color = FLinearColor::Yellow;
		FCanvasTileItem Dot(MiniPos, FVector2D(5, 5), Color);
		Canvas->DrawItem(Dot);
	}
}

FVector2D AAoAHUD::WorldToMinimap(const FVector& WorldLoc) const
{
	float Size = MinimapSize;
	float X = Canvas->ClipX - Size - 20.0f;
	float Y = 20.0f;
	float WorldSize = 6400.0f;
	float MX = X + (WorldLoc.X / WorldSize) * Size;
	float MY = Y + (WorldLoc.Y / WorldSize) * Size;
	return FVector2D(FMath::Clamp(MX, X, X + Size), FMath::Clamp(MY, Y, Y + Size));
}

void AAoAHUD::DrawGameOver()
{
	if (!Canvas) return;

	FCanvasTileItem Overlay(FVector2D(0, 0), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0, 0, 0, 0.6f));
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

	FCanvasTextItem TextItem(FVector2D(Canvas->ClipX * 0.5f - 100, Canvas->ClipY * 0.5f - 30),
		FText::FromString(ResultText), GEngine->GetLargeFont(), FLinearColor(1.0f, 0.9f, 0.4f, 1.0f));
	Canvas->DrawItem(TextItem);

	FString SubText = TEXT("Press ENTER for menu  |  ESC to quit");
	FCanvasTextItem SubItem(FVector2D(Canvas->ClipX * 0.5f - 120, Canvas->ClipY * 0.5f + 20),
		FText::FromString(SubText), GEngine->GetMediumFont(), FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
	Canvas->DrawItem(SubItem);
}

void AAoAHUD::DrawHelpOverlay()
{
	if (!Canvas) return;
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
}
