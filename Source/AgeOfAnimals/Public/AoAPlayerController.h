#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
class AAoAPlayerController : public APlayerController
{
public:
	AAoAPlayerController();
	TArray<TObjectPtr<AActor>> SelectedActors;
	float CurrentZoom = 1500.0f;
	void OnLeftClick();
	void OnRightClick();
	void IssueMoveCommand(const FVector& Location);
};
