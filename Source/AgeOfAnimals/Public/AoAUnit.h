#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "AoAEmpireData.h"
class AAoAResourceNode;
class AAoABuilding;
enum class EUnitState : uint8 { Idle, Moving, Attacking, Gathering, Building, Dead };
class AAoAUnit : public ACharacter
{
public:
	AAoAUnit();
	int32 NetworkID = 0;
	int32 OwnerPlayerId = -1;
	int32 EmpireIndex = 0;
	int32 UnitRoleIndex = 0;
	float CurrentHP = 40.0f;
	float MaxHP = 40.0f;
	float AttackDamage = 6.0f;
	float AttackRange = 60.0f;
	float AttackCooldown = 1.0f;
	bool bIsRanged = false;
	EUnitState State = EUnitState::Idle;
	AActor* CurrentTarget = nullptr;
	int32 CarryAmount = 0;
	EResourceType CarryType = EResourceType::Food;
	TObjectPtr<UStaticMeshComponent> SpriteComponent;
	void CommandMove(const FVector& Location);
	void CommandAttackTarget(AActor* Target);
	void CommandAttackGround(const FVector& Location);
	void CommandGather(AAoAResourceNode* Resource);
	void CommandBuild(UClass* BuildingClass, const FVector& Location);
	void CommandStop();
	void SetOwnerPlayer(int32 PlayerId) { OwnerPlayerId = PlayerId; }
	int32 GetOwnerPlayerId() const { return OwnerPlayerId; }
	void SetEmpireIndex(int32 Empire) { EmpireIndex = Empire; }
	int32 GetNetworkID() const { return NetworkID; }
	bool IsVillager() const { return UnitRoleIndex == 0; }
	bool IsDead() const { return State == EUnitState::Dead; }
	void ApplyDamage(float Damage);
	void Heal(float Amount);
	static AAoAUnit* FindByID(UObject* WorldContext, int32 ID);
};
