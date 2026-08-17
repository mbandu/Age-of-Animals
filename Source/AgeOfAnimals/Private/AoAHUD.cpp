#include "Blueprint/UserWidget.h"
#include "AoAHUD.h"
#include "AoAPlayerController.h"
#include "AoAPlayerState.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

AAoAHUD::AAoAHUD() { PrimaryActorTick.bCanEverTick = true; }

void AAoAHUD::DrawHUD()
{
	Super::DrawHUD();
	auto* MyPC = Cast<AAoAPlayerController>(PlayerOwner);
	if (!MyPC || !Canvas) return;
	auto* MyPS = Cast<AAoAPlayerState>(MyPC->PlayerState);
	
	if (MyPS)
	{
		FString Text = FString::Printf(TEXT("Food: %d  Wood: %d  Stone: %d  Pop: %d/%d  [Age of Animals]"), MyPS->Food, MyPS->Wood, MyPS->Stone, MyPS->PopulationUsed, MyPS->PopulationCap);
		FCanvasTextItem TextItem(FVector2D(10, 10), FText::FromString(Text), GEngine->GetSmallFont(), FLinearColor::White);
		Canvas->DrawItem(TextItem);
	}

	if (!MyPC->SelectedActors.IsEmpty())
	{
		FString SelText = FString::Printf(TEXT("Selected: %d units"), MyPC->SelectedActors.Num());
		FCanvasTextItem SelItem(FVector2D(10, Canvas->ClipY - 40), FText::FromString(SelText), GEngine->GetSmallFont(), FLinearColor::Green);
		Canvas->DrawItem(SelItem);
	}

	FString Help = TEXT("Left-click: Select  |  Right-click: Move  |  Edge of screen: Pan camera");
	FCanvasTextItem HelpItem(FVector2D(10, Canvas->ClipY - 20), FText::FromString(Help), GEngine->GetSmallFont(), FLinearColor(0.5f, 0.5f, 0.5f, 0.7f));
	Canvas->DrawItem(HelpItem);
}

void AAoAHUD::DrawSelectionBox() {}
void AAoAHUD::DrawResourceBar() {}
void AAoAHUD::DrawMinimap() {}
void AAoAHUD::DrawBuildPanel() {}
void AAoAHUD::DrawSelectionPanel() {}
void AAoAHUD::DrawBuildingPanel() {}
void AAoAHUD::DrawGameOver() {}
void AAoAHUD::DrawHelpOverlay() {}
void AAoAHUD::InitializeHUD() {}
void AAoAHUD::ShowWidget(TSubclassOf<UUserWidget> W) {}
void AAoAHUD::RemoveMenuWidgets() {}
void AAoAHUD::DrawMinimapToTexture() {}
FVector2D AAoAHUD::WorldToMinimap(const FVector& W) const { return FVector2D(0,0); }


