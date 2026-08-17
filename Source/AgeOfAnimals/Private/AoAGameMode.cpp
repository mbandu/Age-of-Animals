#include "AoAGameMode.h"
#include "AoAGameState.h"
#include "AoAPlayerState.h"
#include "AoAPlayerController.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAResourceNode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"

AAoAGameMode::AAoAGameMode()
{
	PlayerControllerClass = AAoAPlayerController::StaticClass();
	GameStateClass = AAoAGameState::StaticClass();
	PlayerStateClass = AAoAPlayerState::StaticClass();
}

void AAoAGameMode::SpawnStartingBase(AAoAPlayerController* PC, const FVector& Location)
{
	if (!PC) return;
	auto* PS = Cast<AAoAPlayerState>(PC->PlayerState);
	if (!PS) return;
	UWorld* World = GetWorld();
	if (!World) return;
	GiveStartingResources(PS);
	auto* TC = World->SpawnActor<AAoABuilding>(AAoABuilding::StaticClass(), Location, FRotator::ZeroRotator);
	if (TC) { TC->SetOwnerPlayer(PS->GetPlayerId()); TC->SetEmpireIndex(PS->EmpireIndex); TC->FinishConstruction(); }
	for (int32 i = 0; i < 4; ++i)
	{
		float Angle = i * (6.283f / 4.0f);
		FVector Off(FMath::Cos(Angle)*200.0f, FMath::Sin(Angle)*200.0f, 50.0f);
		auto* V = World->SpawnActor<AAoAUnit>(AAoAUnit::StaticClass(), Location + Off, FRotator::ZeroRotator);
		if (V) { V->SetOwnerPlayer(PS->GetPlayerId()); V->SetEmpireIndex(PS->EmpireIndex); }
	}
}

void AAoAGameMode::CheckWinCondition() {}
void AAoAGameMode::GiveStartingResources(AAoAPlayerState* PS) { if (PS) { PS->Food = StartFood; PS->Wood = StartWood; PS->Stone = StartStone; } }
