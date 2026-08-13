#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AoAHUD.generated.h"

class AAoAPlayerController;
class AAoAPlayerState;
class UAoAGameInstance;

/**
 * RTS HUD — renders the selection drag box, resource bar, minimap,
 * build panel, unit selection panel, and empire selection UI using Slate/UMG.
 * DrawHUD is called every frame to render the in-game UI overlay.
 */
UCLASS(BlueprintType)
class AAoAHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAoAHUD();

	virtual void DrawHUD() override;

	// ---- HUD Drawing ----
	void DrawSelectionBox();
	void DrawResourceBar();
	void DrawMinimap();
	void DrawBuildPanel();
	void DrawSelectionPanel();
	void DrawBuildingPanel();
	void DrawGameOver();
	void DrawHelpOverlay();

	// ---- Empire Selection (pre-game lobby) ----
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowEmpireSelection();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowLobby();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowMainMenu();

	// UMG Widget classes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> EmpireSelectWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> BuildPanelWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> CurrentHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> CurrentMenuWidget;

	// Minimap rendering
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	int32 MinimapSize = 200;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	FVector2D MinimapPosition = FVector2D(20, 20);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void InitializeHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowWidget(TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RemoveMenuWidgets();

protected:
	// Texture for minimap rendering
	UTexture2D* MinimapTexture = nullptr;

	// Cached references
	AAoAPlayerController* PC = nullptr;
	AAoAPlayerState* PS = nullptr;
	UAoAGameInstance* GI = nullptr;

	void DrawMinimapToTexture();
	FVector2D WorldToMinimap(const FVector& WorldLoc) const;
};
