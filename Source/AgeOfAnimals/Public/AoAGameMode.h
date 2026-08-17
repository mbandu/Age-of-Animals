#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AoAGameMode.generated.h"

class AAoAPlayerController;
class AAoAPlayerState;
class AAoAUnit;
class AAoABuilding;
class AAoAResourceNode;

UENUM(BlueprintType)
enum class EMatchType : uint8 { Skirmish, LANHost, LANJoin, OnlineMatch };

UCLASS(BlueprintType)
class AAoAGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAoAGameMode();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup") int32 StartFood = 200;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup") int32 StartWood = 150;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup") int32 StartStone = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup") int32 MaxPlayers = 5;
	UPROPERTY(BlueprintReadWrite, Category = "Match") EMatchType MatchType = EMatchType::Skirmish;
	UPROPERTY(BlueprintReadWrite, Category = "Match") int32 RandomSeed = 0;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	void SpawnStartingBase(AAoAPlayerController* PC, const FVector& Location);
	void CheckWinCondition();
protected:
	TArray<FVector> UsedSpawnLocations;
	void GiveStartingResources(AAoAPlayerState* PS);
};
