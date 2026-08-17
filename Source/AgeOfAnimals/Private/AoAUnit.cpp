#include "AoAUnit.h"
#include "AoAResourceNode.h"
#include "AoABuilding.h"
#include "Engine/StaticMesh.h"

AAoAUnit::AAoAUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	SpriteComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sprite"));
	if (RootComponent) { SpriteComponent->SetupAttachment(RootComponent); }
	SpriteComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 1.0f));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAoAUnit::ApplyDamage(float D) { CurrentHP -= D; if (CurrentHP <= 0) { CurrentHP = 0; SetLifeSpan(3.0f); } }
void AAoAUnit::Heal(float A) {}
void AAoAUnit::CommandMove(const FVector& L) {}
void AAoAUnit::CommandStop() {}
void AAoAUnit::CommandAttackTarget(AActor* T) {}
void AAoAUnit::CommandAttackGround(const FVector& L) {}
void AAoAUnit::CommandGather(AAoAResourceNode* R) {}
void AAoAUnit::CommandBuild(UClass* B, const FVector& L) {}
AAoAUnit* AAoAUnit::FindByID(UObject* W, int32 I) { return nullptr; }
