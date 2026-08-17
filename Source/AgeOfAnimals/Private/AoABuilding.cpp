#include "AoABuilding.h"
#include "Engine/StaticMesh.h"

AAoABuilding::AAoABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
}

void AAoABuilding::StartConstruction() { BuildingState = EBuildingState::UnderConstruction; }
void AAoABuilding::FinishConstruction() { BuildingState = EBuildingState::Complete; ConstructionProgress = 1.0f; CurrentHP = MaxHP; }
void AAoABuilding::AddConstructionProgress(float D) { ConstructionProgress += D/15.0f; if (ConstructionProgress >= 1.0f) FinishConstruction(); }
void AAoABuilding::TrainUnit(UClass* U) { TrainQueue.Add(U); }
void AAoABuilding::SetRallyPoint(const FVector& L) { RallyPoint = L; bRallySet = true; }
void AAoABuilding::ApplyDamage(float D) { CurrentHP -= D; if (CurrentHP <= 0) { CurrentHP = 0; SetLifeSpan(2.0f); } }
