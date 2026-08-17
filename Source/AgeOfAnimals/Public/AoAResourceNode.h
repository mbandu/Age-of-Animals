#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "AoAEmpireData.h"
class AAoAResourceNode : public AActor
{
public:
	AAoAResourceNode();
	EResourceType ResourceType = EResourceType::Wood;
	int32 RemainingAmount = 400;
	int32 MaxAmount = 400;
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	void Harvest(int32 Amount);
	int32 GetRemainingAmount() const { return RemainingAmount; }
	EResourceType GetResourceType() const { return ResourceType; }
	bool IsDepleted() const { return RemainingAmount <= 0; }
	void Initialize(EResourceType Type, int32 Amount);
};
