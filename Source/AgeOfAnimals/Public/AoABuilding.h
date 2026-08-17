#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoABuildingData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "AoABuilding.generated.h"

class AAoAUnit;
class UAoABuildingData;

UENUM(BlueprintType)
enum class EBuildingState : uint8
{
	Placing,
	UnderConstruction,
	Complete,
	Destroyed,
};

/**
 * Base building actor for all animal empires. Handles construction progress,
 * unit training queues, rally points, and defensive attacks (towers).
 */
UCLASS(BlueprintType)
class AAoABuilding : public AActor
{
	GENERATED_BODY()

public:
	AAoABuilding();

	// ---- Identity ----
	UPROPERTY(BlueprintReadOnly, Category = "Building")
	int32 OwnerPlayerId = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Building")
	int32 EmpireIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Building")
	int32 BuildingRoleIndex = 0;

	// ---- Stats ----
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentHP = 1200.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxHP = 1200.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float AttackRange = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	bool bHasAttack = false;

	// ---- Construction ----
	UPROPERTY(BlueprintReadOnly, Category = "Construction")
	EBuildingState BuildingState = EBuildingState::Complete;

	UPROPERTY(BlueprintReadOnly, Category = "Construction")
	float ConstructionProgress = 0.0f; // 0 to 1

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Construction")
	float ConstructionTime = 15.0f;

	// ---- Training Queue ----
	UPROPERTY(BlueprintReadOnly, Category = "Training")
	TArray<UClass*> TrainQueue;

	UPROPERTY(BlueprintReadOnly, Category = "Training")
	float CurrentTrainProgress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Training")
	float CurrentTrainTime = 0.0f;

	// ---- Rally Point ----
	UPROPERTY(BlueprintReadOnly, Category = "Rally")
	FVector RallyPoint = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Rally")
	bool bRallySet = false;

	// ---- Visuals ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<class UDecalComponent> SelectionDecal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UStaticMesh* BuildingMesh;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	// ---- Interface ----
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetOwnerPlayer(int32 PlayerId) { OwnerPlayerId = PlayerId; }

	UFUNCTION(BlueprintCallable, Category = "Building")
	int32 GetOwnerPlayerId() const { return OwnerPlayerId; }

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetEmpireIndex(int32 Empire) { EmpireIndex = Empire; }

	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartConstruction();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void FinishConstruction();

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool IsComplete() const { return BuildingState == EBuildingState::Complete; }

	UFUNCTION(BlueprintCallable, Category = "Building")
	void AddConstructionProgress(float DeltaProgress);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void TrainUnit(UClass* UnitClass);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetRallyPoint(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void ApplyDamage(float Damage);

protected:
	float AttackTimer = 0.0f;

	void UpdateTraining(float DeltaTime);
	void UpdateDefense(float DeltaTime);
	void SpawnTrainedUnit(UClass* UnitClass);
	AActor* FindEnemyInRadius() const;
};
