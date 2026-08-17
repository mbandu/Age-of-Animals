#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
enum class EGamePhase : uint8 { Preparing, Playing, Ended };
class AAoAGameState : public AGameStateBase
{
public:
	EGamePhase Phase = EGamePhase::Preparing;
	int32 WinnerPlayerId = -1;
	float MatchTime = 0.0f;
	FString MatchSeed;
};
