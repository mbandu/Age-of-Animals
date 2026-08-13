#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AoAUnitAIController.generated.h"

/**
 * Minimal AI controller assigned to each unit for navigation.
 * The PlayerController/AIController issues commands to the unit,
 * which uses this controller to navigate via UE's NavigationSystem.
 */
UCLASS(BlueprintType)
class AAoAUnitAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAoAUnitAIController();

	virtual void OnPossess(APawn* InPawn) override;
};
