#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
class AAoAPlayerController;
class AAoAPlayerState;
enum class EMatchType : uint8 { Skirmish, LANHost, LANJoin, OnlineMatch };
class AAoAGameMode : public AGameModeBase
{
public:
	AAoAGameMode();
	int32 StartFood = 200;
	int32 StartWood = 150;
	int32 StartStone = 100;
	int32 MaxPlayers = 5;
	EMatchType MatchType = EMatchType::Skirmish;
	int32 RandomSeed = 0;
	void SpawnStartingBase(AAoAPlayerController* PC, const FVector& Location);
	void CheckWinCondition();
protected:
	TArray<FVector> UsedSpawnLocations;
	void GiveStartingResources(AAoAPlayerState* PS);
};
