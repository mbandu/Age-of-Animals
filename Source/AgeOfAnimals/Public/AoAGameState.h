#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AoAGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8 { Preparing, Playing, Ended };

UCLASS(BlueprintType)
class AAoAGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Game") EGamePhase Phase = EGamePhase::Preparing;
	UPROPERTY(BlueprintReadOnly, Category = "Game") int32 WinnerPlayerId = -1;
	UPROPERTY(BlueprintReadOnly, Category = "Game") float MatchTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Game") FString MatchSeed;
	virtual void Tick(float DeltaTime) override;
};
