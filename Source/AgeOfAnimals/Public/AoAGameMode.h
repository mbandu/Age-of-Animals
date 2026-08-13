#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AoAGameMode.generated.h"

class AAoAGameState;
class AAoAPlayerState;
class AAoAPlayerController;
class UAoAEmpireData;
class AAoAUnit;
class AAoABuilding;

UENUM(BlueprintType)
enum class EMatchType : uint8
{
	Skirmish    UMETA(DisplayName = "Skirmish vs AI"),
	LANHost     UMETA(DisplayName = "LAN Host"),
	LANJoin     UMETA(DisplayName = "LAN Join"),
	OnlineMatch UMETA(DisplayName = "Online Match"),
};

/**
 * Core RTS game mode — handles player setup, starting conditions, win/lose
 * detection, and spawning starting units/buildings for each empire.
 */
UCLASS(BlueprintType)
class AAoAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAoAGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AAoAPlayerState> AoAPlayerStateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AAoAPlayerController> AoAPlayerControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AAoAGameState> AoAGameStateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	int32 StartFood = 200;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	int32 StartWood = 150;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	int32 StartStone = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	int32 StartVillagers = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	int32 MaxPlayers = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AAoAUnit> VillagerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AAoABuilding> TownCenterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
	FIntPoint MapSize = FIntPoint(64, 64);

	UPROPERTY(BlueprintReadWrite, Category = "Match")
	EMatchType MatchType = EMatchType::Skirmish;

	UPROPERTY(BlueprintReadWrite, Category = "Match")
	uint32 RandomSeed = 0;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	/** Spawns the starting town center + villagers for a player */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SpawnStartingBase(AAoAPlayerController* PC, const FVector& Location);

	/** Called when a player has no buildings or units left */
	void NotifyPlayerEliminated(int32 PlayerId);

	/** Checks if only one player remains alive */
	void CheckWinCondition();

	/** Finds a spawn point for a new player that is spread out from existing ones */
	FVector FindSpawnLocation() const;

protected:
	TArray<FVector> UsedSpawnLocations;

	void GiveStartingResources(AAoAPlayerState* PS);
};
