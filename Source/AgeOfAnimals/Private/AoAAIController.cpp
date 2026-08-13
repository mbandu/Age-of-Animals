#include "EngineUtils.h"
#include "AoAAIController.h"
#include "AoAPlayerState.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAResourceNode.h"
#include "AoAGameMode.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AAoAAIController::AAoAAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsAI = true;
}

void AAoAAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ThinkTimer -= DeltaTime;
	if (ThinkTimer > 0.0f) return;
	ThinkTimer = ThinkInterval;

	AttackTimer -= ThinkInterval;

	ThinkEconomy();
	ThinkMilitary();
	ThinkAggression();
}

TArray<AAoAUnit*> AAoAAIController::GetMyUnits(int32 RoleFilter) const
{
	TArray<AAoAUnit*> Result;
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return Result;
	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() != PS->GetPlayerId()) continue;
		if (RoleFilter >= 0 && It->UnitRoleIndex != RoleFilter) continue;
		Result.Add(*It);
	}
	return Result;
}

TArray<AAoABuilding*> AAoAAIController::GetMyBuildings(int32 RoleFilter) const
{
	TArray<AAoABuilding*> Result;
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return Result;
	for (TActorIterator<AAoABuilding> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() != PS->GetPlayerId()) continue;
		if (RoleFilter >= 0 && It->BuildingRoleIndex != RoleFilter) continue;
		Result.Add(*It);
	}
	return Result;
}

void AAoAAIController::ThinkEconomy()
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return;

	auto Villagers = GetMyUnits(0); // role 0 = villager
	auto Dens = GetMyBuildings(0);  // role 0 = town center

	// Assign idle villagers to gather
	for (auto* V : Villagers)
	{
		if (V->State == EUnitState::Idle)
		{
			// Find nearest resource
			AAoAResourceNode* Best = nullptr;
			float BestDist = FLT_MAX;
			for (TActorIterator<AAoAResourceNode> It(GetWorld()); It; ++It)
			{
				if (It->IsDepleted()) continue;
				float D = FVector::Dist2D(V->GetActorLocation(), It->GetActorLocation());
				if (D < BestDist) { BestDist = D; Best = *It; }
			}
			if (Best)
				V->CommandGather(Best);
		}
	}

	// Train more villagers if we have fewer than 14
	if (Villagers.Num() < 14 && !Dens.IsEmpty())
	{
		auto* TC = Dens[0];
		if (TC->TrainQueue.IsEmpty())
		{
			// TC->TrainUnit(VillagerClass);
			// In a complete build, get the villager class from the empire data
		}
	}

	// Build houses if population is near cap
	int32 PopSpace = PS->PopulationCap - PS->PopulationUsed;
	auto Houses = GetMyBuildings(1);
	if (PopSpace <= 3 && Houses.Num() < 8 && !Villagers.IsEmpty())
	{
		// Find a build location near the town center
		if (!Dens.IsEmpty())
		{
			FVector BuildLoc = Dens[0]->GetActorLocation() + FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), 0);
			// Villagers[0]->CommandBuild(HouseClass, BuildLoc);
		}
	}
}

void AAoAAIController::ThinkMilitary()
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return;

	auto Barracks = GetMyBuildings(2);
	auto Villagers = GetMyUnits(0);

	// Build first barracks if we have enough villagers
	if (Barracks.IsEmpty() && Villagers.Num() >= 6)
	{
		if (!Villagers.IsEmpty())
		{
			FVector BuildLoc = Villagers[0]->GetActorLocation() + FVector(300, 300, 0);
			// Villagers[0]->CommandBuild(BarracksClass, BuildLoc);
		}
	}

	// Train army from barracks
	for (auto* Brk : Barracks)
	{
		if (Brk->TrainQueue.IsEmpty())
		{
			// Train warrior or archer
			// Brk->TrainUnit(FMath::RandBool() ? WarriorClass : ArcherClass);
		}
	}
}

void AAoAAIController::ThinkAggression()
{
	auto Army = GetMyUnits(1); // warriors
	Army.Append(GetMyUnits(2)); // archers
	Army.Append(GetMyUnits(3)); // specials

	if (Army.Num() >= 8 && AttackTimer <= 0)
	{
		AttackTimer = 30.0f;
		LaunchAttack();
	}
}

void AAoAAIController::LaunchAttack()
{
	auto Army = GetMyUnits(1);
	Army.Append(GetMyUnits(2));
	Army.Append(GetMyUnits(3));

	AAoABuilding* Target = FindNearestEnemyBuilding();
	if (!Target) return;

	for (auto* U : Army)
	{
		U->CommandAttackTarget(Target);
	}
}

AAoABuilding* AAoAAIController::FindNearestEnemyBuilding() const
{
	auto* PS = Cast<AAoPlayerState>(PlayerState);
	if (!PS) return nullptr;

	AAoABuilding* Best = nullptr;
	float BestDist = FLT_MAX;
	FVector MyCenter = FVector::ZeroVector;

	auto MyUnits = GetMyUnits();
	for (auto* U : MyUnits)
		MyCenter += U->GetActorLocation();
	if (!MyUnits.IsEmpty())
		MyCenter /= MyUnits.Num();

	for (TActorIterator<AAoABuilding> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() == PS->GetPlayerId()) continue;
		float D = FVector::Dist2D(MyCenter, It->GetActorLocation());
		if (D < BestDist) { BestDist = D; Best = *It; }
	}
	return Best;
}

