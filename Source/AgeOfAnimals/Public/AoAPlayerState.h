#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AoAEmpireData.h"
class AAoAPlayerState : public APlayerState
{
public:
	AAoAPlayerState();
	int32 EmpireIndex = 0;
	bool bIsAI = false;
	int32 Food = 200;
	int32 Wood = 150;
	int32 Stone = 100;
	int32 Gold = 0;
	int32 PopulationUsed = 0;
	int32 PopulationCap = 10;
	bool bIsAlive = true;
	int32 CurrentAge = 0;
	int32 Kills = 0;
	int32 UnitsLost = 0;
	bool SpendResources(int32 InFood, int32 InWood, int32 InStone, int32 InGold);
	void AddResource(EResourceType Type, int32 Amount);
	bool CanAfford(int32 InFood, int32 InWood, int32 InStone, int32 InGold = 0) const;
	bool HasPopulationSpace(int32 Cost) const { return PopulationUsed + Cost <= PopulationCap; }
};
