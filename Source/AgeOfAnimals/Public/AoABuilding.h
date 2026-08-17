#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
enum class EBuildingState : uint8 { Placing, UnderConstruction, Complete, Destroyed };
class AAoABuilding : public AActor
{
public:
	AAoABuilding();
	int32 OwnerPlayerId = -1;
	int32 EmpireIndex = 0;
	int32 BuildingRoleIndex = 0;
	float CurrentHP = 1200.0f;
	float MaxHP = 1200.0f;
	EBuildingState BuildingState = EBuildingState::Complete;
	float ConstructionProgress = 0.0f;
	TArray<UClass*> TrainQueue;
	FVector RallyPoint = FVector::ZeroVector;
	bool bRallySet = false;
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	void SetOwnerPlayer(int32 PlayerId) { OwnerPlayerId = PlayerId; }
	int32 GetOwnerPlayerId() const { return OwnerPlayerId; }
	void SetEmpireIndex(int32 Empire) { EmpireIndex = Empire; }
	void StartConstruction();
	void FinishConstruction();
	bool IsComplete() const { return BuildingState == EBuildingState::Complete; }
	void AddConstructionProgress(float DeltaProgress);
	void TrainUnit(UClass* UnitClass);
	void SetRallyPoint(const FVector& Location);
	void ApplyDamage(float Damage);
};
