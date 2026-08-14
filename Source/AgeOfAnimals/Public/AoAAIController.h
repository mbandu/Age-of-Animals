#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AoAAIController.generated.h"

class AAoAUnit;
class AAoABuilding;
class AAoAPlayerState;

/**
 * AI opponent controller — manages an AI player's economy, production,
 * and military strategy. Operates on the server side with the same
 * interface as a human player controller.
 *
 * AI strategy phases:
 * 1. Economy: gather resources, build houses, train villagers
 * 2. Military: build barracks, train army
 * 3. Aggression: attack enemy bases with accumulated army
 */
UCLASS(BlueprintType)
class AAoAAIController : public APlayerController
{
	GENERATED_BODY()

public:
	AAoAAIController();

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	float ThinkInterval = 2.0f;

	virtual void Tick(float DeltaTime) override;

protected:
	float ThinkTimer = 0.0f;
	float AttackTimer = 45.0f; // seconds before first attack

	void ThinkEconomy();
	void ThinkMilitary();
	void ThinkAggression();

	TArray<AAoAUnit*> GetMyUnits(int32 RoleFilter = -1) const;
	TArray<AAoABuilding*> GetMyBuildings(int32 RoleFilter = -1) const;
	AAoABuilding* FindNearestEnemyBuilding() const;

	void TrainVillagers();
	void AssignVillagersToGather();
	void BuildHouses();
	void BuildBarracks();
	void TrainArmy();
	void LaunchAttack();
};

