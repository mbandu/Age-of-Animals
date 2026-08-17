#include "AoAGameMode.h"
#include "AoAGameState.h"
#include "AoAPlayerState.h"
#include "AoAPlayerController.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAResourceNode.h"
#include "AoAGameInstance.h"
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

void AAoAGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (RandomSeed == 0) RandomSeed = FMath::Rand();
}

void AAoAGameMode::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) return;

	// Spawn ground
	AStaticMeshActor* Ground = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator);
	if (Ground)
	{
		Ground->SetActorScale3D(FVector(50,50,1));
		UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (PlaneMesh) Ground->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
		Ground->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// Spawn resources
	for (int32 i = 0; i < 10; ++i)
	{
		float X = FMath::RandRange(-2000, 2000);
		float Y = FMath::RandRange(-2000, 2000);
		AAoAResourceNode* Res = World->SpawnActor<AAoAResourceNode>(AAoAResourceNode::StaticClass(), FVector(X,Y,0), FRotator::ZeroRotator);
		if (Res) { Res->RemainingAmount = 400; Res->MaxAmount = 400; }
	}

	// Spawn starting base
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (auto* PC = Cast<AAoAPlayerController>(It->Get()))
		{
			SpawnStartingBase(PC, FVector(0, 0, 100));
			break;
		}
	}

	if (auto* GS = Cast<AAoAGameState>(GameState)) { GS->Phase = EGamePhase::Playing; }
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
