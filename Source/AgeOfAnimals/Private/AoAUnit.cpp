#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "AoAUnit.h"
#include "AoAUnitAIController.h"
#include "AIController.h"
#include "AoAPlayerState.h"
#include "AoAResourceNode.h"
#include "AoABuilding.h"
#include "AoAEmpireData.h"
#include "AoAUnitData.h"
#include "AoAGameState.h"
#include "AoAGameMode.h"
#include "Net/UnrealNetwork.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

AAoAUnit::AAoAUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Sprite component
	SpriteComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sprite"));
	if (RootComponent)
	{
		SpriteComponent->SetupAttachment(RootComponent);
	}
}

void AAoAUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAoAUnit, NetworkID);
	DOREPLIFETIME(AAoAUnit, OwnerPlayerId);
	DOREPLIFETIME(AAoAUnit, EmpireIndex);
	DOREPLIFETIME(AAoAUnit, UnitRoleIndex);
	DOREPLIFETIME(AAoAUnit, CurrentHP);
	DOREPLIFETIME(AAoAUnit, State);
	DOREPLIFETIME(AAoAUnit, CurrentTarget);
	DOREPLIFETIME(AAoAUnit, CarryAmount);
	DOREPLIFETIME(AAoAUnit, CarryType);
}

void AAoAUnit::BeginPlay()
{
	Super::BeginPlay();
	
	// Configure character movement for RTS
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->bUseControllerDesiredRotation = false;
		MoveComp->MaxWalkSpeed = 300.0f;
		MoveComp->bConstrainToPlane = true;
		MoveComp->SetPlaneConstraintNormal(FVector(0, 0, 1));
	}
	bUseControllerRotationYaw = false;
	
	ResolveStats();
	CurrentHP = MaxHP;

	// Assign a unique network ID if none set
	if (NetworkID == 0 && HasAuthority())
	{
		NetworkID = static_cast<uint32>(GetUniqueID());
	}
}

void AAoAUnit::ResolveStats()
{
	// Load the unit data asset for this empire+role
	auto* GI = GetGameInstance();
	// In a complete build, we'd load the data asset and apply empire bonuses.
	// The empire bonus multipliers are applied here.
	const float EmpireBonuses[5][5] = {
		// [empire][hp, speed, attack, range, gather]
		{1.20f, 1.00f, 1.00f, 1.00f, 1.00f}, // Panda
		{1.00f, 1.25f, 1.00f, 1.00f, 1.00f}, // Bunny
		{1.00f, 1.00f, 1.00f, 1.00f, 1.15f}, // Monkey
		{1.00f, 1.00f, 1.30f, 1.00f, 1.00f}, // Gorilla
		{1.00f, 1.05f, 1.00f, 1.20f, 1.00f}, // Cat
	};

	int32 Emp = FMath::Clamp(EmpireIndex, 0, 4);
	float HpMult = EmpireBonuses[Emp][0];
	float SpdMult = EmpireBonuses[Emp][1];
	float AtkMult = EmpireBonuses[Emp][2];
	float RngMult = EmpireBonuses[Emp][3];
	float GathMult = EmpireBonuses[Emp][4];

	// Base stats by role
	switch (UnitRoleIndex)
	{
	case 0: MaxHP = 40.0f * HpMult;  AttackDamage = 6.0f * AtkMult; AttackRange = 60.0f * RngMult; bIsRanged = false; break;
	case 1: MaxHP = 90.0f * HpMult;  AttackDamage = 12.0f * AtkMult; AttackRange = 60.0f * RngMult; bIsRanged = false; break;
	case 2: MaxHP = 55.0f * HpMult;  AttackDamage = 9.0f * AtkMult; AttackRange = 450.0f * RngMult; bIsRanged = true; break;
	case 3: MaxHP = 120.0f * HpMult; AttackDamage = 18.0f * AtkMult; AttackRange = 80.0f * RngMult; bIsRanged = false; break;
	}

	GetCharacterMovement()->MaxWalkSpeed = 300.0f * SpdMult;
	AttackCooldown = (UnitRoleIndex == 2) ? 1.2f : 1.0f;
}

void AAoAUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdateState(DeltaTime);
	}

	UpdateAnimation();
}

void AAoAUnit::UpdateState(float DeltaTime)
{
	switch (State)
	{
	case EUnitState::Idle:
		// Look for nearby enemies if we have no target
		break;
	case EUnitState::Moving:
		UpdateMovement(DeltaTime);
		break;
	case EUnitState::Attacking:
		UpdateCombat(DeltaTime);
		break;
	case EUnitState::Gathering:
		UpdateGathering(DeltaTime);
		break;
	case EUnitState::Building:
		UpdateBuilding(DeltaTime);
		break;
	case EUnitState::Dead:
		break;
	}
}

void AAoAUnit::UpdateMovement(float DeltaTime)
{
	if (GetCharacterMovement()->Velocity.IsNearlyZero())
	{
		// Reached destination
		State = EUnitState::Idle;
	}
}

void AAoAUnit::UpdateCombat(float DeltaTime)
{
	if (!CurrentTarget || CurrentTarget->IsActorBeingDestroyed())
	{
		State = EUnitState::Idle;
		CurrentTarget = nullptr;
		return;
	}

	float Dist = FVector::Dist2D(GetActorLocation(), CurrentTarget->GetActorLocation());

	if (Dist > AttackRange)
	{
		// Move closer
		{
			if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(CurrentTarget->GetActorLocation(), 0.1f, true, true, true, false, 0, false); }
		}
		return;
	}

	// In range — attack
	AttackTimer -= DeltaTime;
	if (AttackTimer <= 0.0f)
	{
		AttackTimer = AttackCooldown;

		if (bIsRanged)
		{
			SpawnProjectile(CurrentTarget);
		}
		else
		{
			// Melee: apply damage directly
			if (auto* EnemyUnit = Cast<AAoAUnit>(CurrentTarget))
				EnemyUnit->ApplyDamage(AttackDamage);
			else if (auto* EnemyBld = Cast<AAoABuilding>(CurrentTarget))
				EnemyBld->ApplyDamage(AttackDamage);
		}
	}
}

void AAoAUnit::UpdateGathering(float DeltaTime)
{
	if (!CurrentResourceNode.IsValid())
	{
		// Find nearest resource of the same type or go drop off
		if (CarryAmount > 0)
		{
			AAoABuilding* DropOff = FindNearestDropOff();
			if (DropOff)
			{
				float Dist = FVector::Dist2D(GetActorLocation(), DropOff->GetActorLocation());
				if (Dist <= 150.0f)
				{
					DropOffResources();
				}
				else
				{
						if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(DropOff->GetActorLocation(), 0.1f, true, true, true, false, 0, false); }
				}
			}
		}
		else
		{
			State = EUnitState::Idle;
		}
		return;
	}

	float Dist = FVector::Dist2D(GetActorLocation(), CurrentResourceNode->GetActorLocation());
	if (Dist > 120.0f)
	{
		// Move to resource
		if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(CurrentResourceNode->GetActorLocation(), 0.1f, true, true, true, false, 0, false); }
		return;
	}

	// At resource — gather
	GatherTimer -= DeltaTime;
	if (GatherTimer <= 0.0f)
	{
		GatherTimer = 1.0f;
		int32 Amount = FMath::Min(5, CurrentResourceNode->GetRemainingAmount());
		CurrentResourceNode->Harvest(Amount);
		CarryAmount += Amount;
		CarryType = CurrentResourceNode->GetResourceType();

		int32 Cap = (UnitRoleIndex == 0) ? 10 : 0;
		if (CarryAmount >= Cap)
		{
			// Head back to drop off
			CurrentResourceNode = nullptr;
		}
	}
}

void AAoAUnit::UpdateBuilding(float DeltaTime)
{
	if (!PendingBuilding.IsValid())
	{
		State = EUnitState::Idle;
		return;
	}

	float Dist = FVector::Dist2D(GetActorLocation(), PendingBuilding->GetActorLocation());
	if (Dist > 200.0f)
	{
		if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(PendingBuilding->GetActorLocation(), 0.1f, true, true, true, false, 0, false); }
		return;
	}

	// Contribute to construction
	PendingBuilding->AddConstructionProgress(10.0f * DeltaTime);
	if (PendingBuilding->IsComplete())
	{
		PendingBuilding = nullptr;
		State = EUnitState::Idle;
	}
}

void AAoAUnit::UpdateAnimation()
{
	// Determine facing angle from velocity or target
	float Angle = 0.0f;
	FVector Vel = GetVelocity();
	if (!Vel.IsNearlyZero())
	{
		Angle = FMath::Atan2(Vel.Y, Vel.X) * 180.0f / PI;
	}
	PlayFlipbookForState(State, Angle);
}

void AAoAUnit::PlayFlipbookForState(EUnitState InState, float FacingAngle)
{
	// In a complete build, this selects the correct flipbook from the UnitData
	// based on state and 8-directional facing. The flipbooks are generated by
	// AoASpriteGenerator and assigned to the data assets.
	// For now, this is a placeholder that could use a default flipbook.
}

// ---- Commands ----
void AAoAUnit::CommandMove(const FVector& Location)
{
	if (!HasAuthority()) return;
	State = EUnitState::Moving;
	CurrentTarget = nullptr;
	CurrentResourceNode = nullptr;

		if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(Location, 0.1f, true, true, true, false, 0, false); }
}

void AAoAUnit::CommandAttackTarget(AActor* Target)
{
	if (!HasAuthority() || !Target) return;
	State = EUnitState::Attacking;
	CurrentTarget = Target;
}

void AAoAUnit::CommandAttackGround(const FVector& Location)
{
	if (!HasAuthority()) return;
	State = EUnitState::Moving;
	// Move to location, then attack anything nearby
		if (AAIController* AIC = Cast<AAIController>(GetController())) { AIC->MoveToLocation(Location, 0.1f, true, true, true, false, 0, false); }
}

void AAoAUnit::CommandGather(AAoAResourceNode* Resource)
{
	if (!HasAuthority() || !Resource) return;
	State = EUnitState::Gathering;
	CurrentResourceNode = Resource;
	CurrentTarget = nullptr;
}

void AAoAUnit::CommandBuild(UClass* BuildingClass, const FVector& Location)
{
	if (!HasAuthority()) return;
	PendingBuildingClass = BuildingClass;
	BuildLocation = Location;

	// Spawn the building (unconstructed)
	FActorSpawnParameters SpawnParams;
	auto* NewBld = GetWorld()->SpawnActor<AAoABuilding>(BuildingClass, Location, FRotator::ZeroRotator, SpawnParams);
	if (NewBld)
	{
		NewBld->SetOwnerPlayer(OwnerPlayerId);
		NewBld->SetEmpireIndex(EmpireIndex);
		NewBld->StartConstruction();
		PendingBuilding = NewBld;
		State = EUnitState::Building;
	}
}

void AAoAUnit::CommandStop()
{
	if (!HasAuthority()) return;
	State = EUnitState::Idle;
	CurrentTarget = nullptr;
	CurrentResourceNode = nullptr;
	GetCharacterMovement()->StopMovementImmediately();
}

void AAoAUnit::ApplyDamage(float Damage)
{
	if (HasAuthority())
	{
		CurrentHP -= Damage;
		if (CurrentHP <= 0.0f)
		{
			CurrentHP = 0.0f;
			State = EUnitState::Dead;
			// Schedule destruction
			SetLifeSpan(3.0f); // corpse remains for 3 seconds
		}
	}
}

void AAoAUnit::Heal(float Amount)
{
	if (HasAuthority())
	{
		CurrentHP = FMath::Min(MaxHP, CurrentHP + Amount);
	}
}

void AAoAUnit::DropOffResources()
{
	if (CarryAmount <= 0) return;

	// Find owner player state and add resources
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		auto* PS = Cast<AAoAPlayerState>(It->Get()->PlayerState);
		if (PS && PS->GetPlayerId() == OwnerPlayerId)
		{
			PS->AddResource(CarryType, CarryAmount);
			break;
		}
	}

	CarryAmount = 0;
	// Return to gathering
	if (CurrentResourceNode.IsValid())
	{
		State = EUnitState::Gathering;
	}
	else
	{
		State = EUnitState::Idle;
	}
}

AAoAResourceNode* AAoAUnit::FindNearestResource(EResourceType Type) const
{
	AAoAResourceNode* Best = nullptr;
	float BestDist = FLT_MAX;
	for (TActorIterator<AAoAResourceNode> It(GetWorld()); It; ++It)
	{
		if (It->GetResourceType() != Type) continue;
		float D = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (D < BestDist) { BestDist = D; Best = *It; }
	}
	return Best;
}

AAoABuilding* AAoAUnit::FindNearestDropOff() const
{
	AAoABuilding* Best = nullptr;
	float BestDist = FLT_MAX;
	for (TActorIterator<AAoABuilding> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() != OwnerPlayerId) continue;
		if (!It->IsComplete()) continue;
		float D = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (D < BestDist) { BestDist = D; Best = *It; }
	}
	return Best;
}

void AAoAUnit::SpawnProjectile(AActor* Target)
{
	// In a complete build, spawn an AoAProjectile actor that travels toward target
	// and applies damage on hit. For now, apply damage directly as fallback.
	if (auto* EnemyUnit = Cast<AAoAUnit>(Target))
		EnemyUnit->ApplyDamage(AttackDamage);
	else if (auto* EnemyBld = Cast<AAoABuilding>(Target))
		EnemyBld->ApplyDamage(AttackDamage);
}

AAoAUnit* AAoAUnit::FindByID(UObject* WorldContext, int32 ID)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;
	for (TActorIterator<AAoAUnit> It(World); It; ++It)
	{
		if (It->GetNetworkID() == ID)
			return *It;
	}
	return nullptr;
}





