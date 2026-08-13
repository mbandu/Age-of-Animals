#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoABuildingData.h"
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
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Building")
	int32 OwnerPlayerId = -1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Building")
	int32 EmpireIndex = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Building")
	int32 BuildingRoleIndex = 0;

	// ---- Stats ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
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
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Construction")
	EBuildingState BuildingState = EBuildingState::Complete;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Construction")
	float ConstructionProgress = 0.0f; // 0 to 1

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Construction")
	float ConstructionTime = 15.0f;

	// ---- Training Queue ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Training")
	TArray<TSubclassOf<AAoAUnit>> TrainQueue;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Training")
	float CurrentTrainProgress = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Training")
	float CurrentTrainTime = 0.0f;

	// ---- Rally Point ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Rally")
	FVector RallyPoint = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Rally")
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
	void TrainUnit(TSubclassOf<AAoAUnit> UnitClass);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetRallyPoint(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void ApplyDamage(float Damage);

protected:
	float AttackTimer = 0.0f;

	void UpdateTraining(float DeltaTime);
	void UpdateDefense(float DeltaTime);
	void SpawnTrainedUnit(TSubclassOf<AAoAUnit> UnitClass);
	AActor* FindEnemyInRadius() const;
};
