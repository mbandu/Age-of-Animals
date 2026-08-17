#include "AoAUnit.h"
#include "AoAResourceNode.h"
#include "AoABuilding.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"

AAoAUnit::AAoAUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	SpriteComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sprite"));
	if (RootComponent) { SpriteComponent->SetupAttachment(RootComponent); }
	SpriteComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 1.0f));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAoAUnit::BeginPlay() { Super::BeginPlay(); UStaticMesh* M = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")); if (M && SpriteComponent) SpriteComponent->SetStaticMesh(M); }
void AAoAUnit::Tick(float D) { Super::Tick(D); }
void AAoAUnit::ApplyDamage(float D) { CurrentHP -= D; if (CurrentHP <= 0) { CurrentHP = 0; SetLifeSpan(3.0f); } }
void AAoAUnit::Heal(float A) {}
void AAoAUnit::CommandMove(const FVector& L) {}
void AAoAUnit::CommandStop() {}
void AAoAUnit::CommandAttackTarget(AActor* T) {}
void AAoAUnit::CommandAttackGround(const FVector& L) {}
void AAoAUnit::CommandGather(AAoAResourceNode* R) {}
void AAoAUnit::CommandBuild(UClass* B, const FVector& L) {}
AAoAUnit* AAoAUnit::FindByID(UObject* W, int32 I) { return nullptr; }
void AAoAUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& P) const { Super::GetLifetimeReplicatedProps(P); }
void AAoAUnit::UpdateState(float D) {}
void AAoAUnit::UpdateMovement(float D) {}
void AAoAUnit::UpdateCombat(float D) {}
void AAoAUnit::UpdateGathering(float D) {}
void AAoAUnit::UpdateBuilding(float D) {}
void AAoAUnit::UpdateAnimation() {}
void AAoAUnit::ResolveStats() {}
void AAoAUnit::DropOffResources() {}
AAoAResourceNode* AAoAUnit::FindNearestResource(EResourceType T) const { return nullptr; }
AAoABuilding* AAoAUnit::FindNearestDropOff() const { return nullptr; }
void AAoAUnit::SpawnProjectile(AActor* T) {}
void AAoAUnit::PlayFlipbookForState(EUnitState S, float A) {}
