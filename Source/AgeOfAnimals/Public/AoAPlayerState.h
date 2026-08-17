#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AoAEmpireData.h"
#include "AoAPlayerState.generated.h"

/**
 * Per-player state: empire choice, resources, population, and alive status.
 * Replicated so every client can see all players' economies.
 */
UCLASS(BlueprintType)
class AAoAPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAoAPlayerState();

	UPROPERTY(BlueprintReadOnly, Category = "Empire")
	int32 EmpireIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Empire")
	bool bIsAI = false;

	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 Food = 200;

	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 Wood = 150;

	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 Stone = 100;

	UPROPERTY(BlueprintReadOnly, Category = "Resources")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Population")
	int32 PopulationUsed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Population")
	int32 PopulationCap = 10;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	bool bIsAlive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 CurrentAge = 0; // 0=Tribal, 1=Bronze, 2=Iron

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 UnitsLost = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Kills() {}

	/** Spend resources — returns true if affordable */
	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool SpendResources(int32 InFood, int32 InWood, int32 InStone, int32 InGold);

	/** Add gathered resources */
	UFUNCTION(BlueprintCallable, Category = "Resources")
	void AddResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool CanAfford(int32 InFood, int32 InWood, int32 InStone, int32 InGold = 0) const;

	UFUNCTION(BlueprintCallable, Category = "Population")
	bool HasPopulationSpace(int32 Cost) const { return PopulationUsed + Cost <= PopulationCap; }
};