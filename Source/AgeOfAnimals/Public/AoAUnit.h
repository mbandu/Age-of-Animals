#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "AoAUnitData.h"
#include "AoAEmpireData.h"
class AAoAUnitAIController;
#include "AoAUnit.generated.h"

class AAoAResourceNode;
class AAoABuilding;
class UAoAProjectileComponent;

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle,
	Moving,
	Attacking,
	Gathering,
	Building,
	Dead,
};

/**
 * Base unit actor for all animal empires. Handles movement (via UE Navigation),
 * combat, resource gathering, construction, and Paper2D flipbook animation.
 * The visual sprite is driven by the SpriteGenerator-created textures.
 *
 * Networked: movement and state are replicated. Owner player is authoritative.
 */
UCLASS(BlueprintType)
class AAoAUnit : public ACharacter
{
	GENERATED_BODY()

public:
	AAoAUnit();

	// ---- Identity ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Unit")
	int32 NetworkID = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Unit")
	int32 OwnerPlayerId = -1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Unit")
	int32 EmpireIndex = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Unit")
	int32 UnitRoleIndex = 0; // 0=villager, 1=warrior, 2=archer, 3=special

	// ---- Stats ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	float CurrentHP = 40.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float MaxHP = 40.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 6.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float AttackRange = 60.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float AttackCooldown = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	bool bIsRanged = false;

	// ---- State ----
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	EUnitState State = EUnitState::Idle;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_CarryData, BlueprintReadOnly, Category = "Gathering")
	int32 CarryAmount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gathering")
	EResourceType CarryType = EResourceType::Food;

	// ---- Visuals ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UPaperFlipbookComponent> SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<class USceneComponent> SelectionRing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UMaterialInterface* SpriteMaterial;

	// ---- Methods ----
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	// Command interface (called on server)
	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandMove(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandAttackTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandAttackGround(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandGather(AAoAResourceNode* Resource);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandBuild(TSubclassOf<AAoABuilding> BuildingClass, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	virtual void CommandStop();

	// Identity helpers
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SetOwnerPlayer(int32 PlayerId) { OwnerPlayerId = PlayerId; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	int32 GetOwnerPlayerId() const { return OwnerPlayerId; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SetEmpireIndex(int32 Empire) { EmpireIndex = Empire; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	int32 GetNetworkID() const { return NetworkID; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	bool IsVillager() const { return UnitRoleIndex == 0; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	bool IsDead() const { return State == EUnitState::Dead; }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	void ApplyDamage(float Damage);

	UFUNCTION(BlueprintCallable, Category = "Unit")
	void Heal(float Amount);

	UFUNCTION()
	void OnRep_CarryData() {}

	// Static lookup by network ID
	UFUNCTION(BlueprintCallable, Category = "Unit", meta = (WorldContext = "WorldContext"))
	static AAoAUnit* FindByID(UObject* WorldContext, int32 ID);

protected:
	// Internal state
	float AttackTimer = 0.0f;
	float GatherTimer = 0.0f;
	TWeakObjectPtr<AAoAResourceNode> CurrentResourceNode;
	TSubclassOf<AAoABuilding> PendingBuildingClass;
	FVector BuildLocation = FVector::ZeroVector;
	TWeakObjectPtr<AAoABuilding> PendingBuilding;

	// State machine
	void UpdateState(float DeltaTime);
	void UpdateMovement(float DeltaTime);
	void UpdateCombat(float DeltaTime);
	void UpdateGathering(float DeltaTime);
	void UpdateBuilding(float DeltaTime);
	void UpdateAnimation();

	// Apply empire bonuses to stats
	void ResolveStats();

	// Drop off resources at nearest building
	void DropOffResources();

	// Find nearest resource of a given type
	AAoAResourceNode* FindNearestResource(EResourceType Type) const;

	// Find nearest drop-off building
	AAoABuilding* FindNearestDropOff() const;

	// Spawn projectile for ranged attack
	void SpawnProjectile(AActor* Target);

	// Play animation based on state and facing
	void PlayFlipbookForState(EUnitState InState, float FacingAngle);
};

