#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AoAGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Preparing UMETA(DisplayName = "Preparing"),
	Playing   UMETA(DisplayName = "Playing"),
	Ended     UMETA(DisplayName = "Ended"),
};

/**
 * Authoritative game state: tracks winner, elapsed time, and match phase.
 * Replicated to all clients.
 */
UCLASS(BlueprintType)
class AAoAGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAoAGameState();

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	EGamePhase Phase = EGamePhase::Preparing;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 WinnerPlayerId = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	float MatchTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	FString MatchSeed;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	/** Called by game mode when a player is eliminated */
	void OnPlayerEliminated(int32 PlayerId);

	/** Called by game mode to check win conditions */
	void CheckWinCondition();
};
