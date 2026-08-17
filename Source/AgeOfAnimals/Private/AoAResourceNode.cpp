#include "AoAResourceNode.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AAoAResourceNode::AAoAResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void AAoAResourceNode::BeginPlay() { Super::BeginPlay(); UStaticMesh* M = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")); if (M && MeshComponent) MeshComponent->SetStaticMesh(M); }
void AAoAResourceNode::Harvest(int32 A) { RemainingAmount = FMath::Max(0, RemainingAmount - A); if (RemainingAmount <= 0) SetLifeSpan(2.0f); }
void AAoAResourceNode::Initialize(EResourceType T, int32 A) { ResourceType = T; MaxAmount = A; RemainingAmount = A; }
void AAoAResourceNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& P) const { Super::GetLifetimeReplicatedProps(P); }
