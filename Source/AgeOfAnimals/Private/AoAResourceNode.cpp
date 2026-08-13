#include "AoAResourceNode.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

AAoAResourceNode::AAoAResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AAoAResourceNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAoAResourceNode, ResourceType);
	DOREPLIFETIME(AAoAResourceNode, RemainingAmount);
	DOREPLIFETIME(AAoAResourceNode, MaxAmount);
}

void AAoAResourceNode::BeginPlay()
{
	Super::BeginPlay();
	if (MaxAmount == 0) MaxAmount = 400;
	if (RemainingAmount == 0) RemainingAmount = MaxAmount;
}

void AAoAResourceNode::Initialize(EResourceType Type, int32 Amount)
{
	ResourceType = Type;
	MaxAmount = Amount;
	RemainingAmount = Amount;

	// Update mesh based on type
	// In a complete build, swap the mesh component's mesh
}

void AAoAResourceNode::Harvest(int32 Amount)
{
	if (!HasAuthority()) return;
	RemainingAmount = FMath::Max(0, RemainingAmount - Amount);
	if (RemainingAmount <= 0)
	{
		// Resource depleted — destroy after a delay
		SetLifeSpan(2.0f);
	}
}
