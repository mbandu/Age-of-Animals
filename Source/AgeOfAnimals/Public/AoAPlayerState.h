#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AoAEmpireData.h"
#include "AoAPlayerState.generated.h"

UCLASS(BlueprintType)
class AAoAPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AAoAPlayerState();
	UPROPERTY(BlueprintReadOnly, Category = "Empire") int32 EmpireIndex = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Empire") bool bIsAI = false;
	UPROPERTY(BlueprintReadOnly, Category = "Resources") int32 Food = 200;
	UPROPERTY(BlueprintReadOnly, Category = "Resources") int32 Wood = 150;
	UPROPERTY(BlueprintReadOnly, Category = "Resources") int32 Stone = 100;
	UPROPERTY(BlueprintReadOnly, Category = "Resources") int32 Gold = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Population") int32 PopulationUsed = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Population") int32 PopulationCap = 10;
	UPROPERTY(BlueprintReadOnly, Category = "Game") bool bIsAlive = true;
	UPROPERTY(BlueprintReadOnly, Category = "Game") int32 CurrentAge = 0;
	UFUNCTION(BlueprintCallable, Category = "Resources") bool SpendResources(int32 InFood, int32 InWood, int32 InStone, int32 InGold);
	UFUNCTION(BlueprintCallable, Category = "Resources") void AddResource(EResourceType Type, int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Resources") bool CanAfford(int32 InFood, int32 InWood, int32 InStone, int32 InGold = 0) const;
};
