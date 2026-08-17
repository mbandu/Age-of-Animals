#include "AoAGameState.h"
void AAoAGameState::Tick(float DeltaTime) { Super::Tick(DeltaTime); if (Phase == EGamePhase::Playing) MatchTime += DeltaTime; }
