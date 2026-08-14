#include "AoAGameState.h"
#include "Net/UnrealNetwork.h"

AAoAGameState::AAoAGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AAoAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAoAGameState, Phase);
	DOREPLIFETIME(AAoAGameState, WinnerPlayerId);
	DOREPLIFETIME(AAoAGameState, MatchTime);
	DOREPLIFETIME(AAoAGameState, MatchSeed);
}

void AAoAGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Phase == EGamePhase::Playing)
	{
		MatchTime += DeltaTime;
	}
}

void AAoAGameState::OnPlayerEliminated(int32 PlayerId)
{
	// Game mode handles elimination detection
}

void AAoAGameState::CheckWinCondition()
{
	if (Phase != EGamePhase::Playing) return;
	// The game mode iterates alive players and sets WinnerPlayerId
}
