#include "EngineUtils.h"
#include "AoABuilding.h"
#include "AoAUnit.h"
#include "AoAPlayerState.h"
#include "AoABuildingData.h"
#include "AoAEmpireData.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

AAoABuilding::AAoABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Resolve base stats based on role
	MaxHP = 1200.0f;
	CurrentHP = MaxHP;
}

void AAoABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAoABuilding, OwnerPlayerId);
	DOREPLIFETIME(AAoABuilding, EmpireIndex);
	DOREPLIFETIME(AAoABuilding, BuildingRoleIndex);
	DOREPLIFETIME(AAoABuilding, CurrentHP);
	DOREPLIFETIME(AAoABuilding, BuildingState);
	DOREPLIFETIME(AAoABuilding, ConstructionProgress);
	DOREPLIFETIME(AAoABuilding, TrainQueue);
	DOREPLIFETIME(AAoABuilding, CurrentTrainProgress);
	DOREPLIFETIME(AAoABuilding, CurrentTrainTime);
	DOREPLIFETIME(AAoABuilding, RallyPoint);
	DOREPLIFETIME(AAoABuilding, bRallySet);
}

void AAoABuilding::BeginPlay()
{
	Super::BeginPlay();

	// Resolve stats from building role
	switch (BuildingRoleIndex)
	{
	case 0: MaxHP = 1200.0f; break; // Town Center
	case 1: MaxHP = 350.0f; break;  // House
	case 2: MaxHP = 700.0f; break;  // Barracks
	case 3: MaxHP = 600.0f; bHasAttack = true; AttackDamage = 10.0f; AttackRange = 500.0f; break; // Tower
	}

	if (BuildingState == EBuildingState::Complete)
		CurrentHP = MaxHP;
}

void AAoABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (BuildingState == EBuildingState::UnderConstruction)
		{
			// Construction progress is advanced by villagers calling AddConstructionProgress
		}

		if (BuildingState == EBuildingState::Complete)
		{
			UpdateTraining(DeltaTime);
			if (bHasAttack)
				UpdateDefense(DeltaTime);
		}
	}
}

void AAoABuilding::StartConstruction()
{
	if (!HasAuthority()) return;
	BuildingState = EBuildingState::UnderConstruction;
	ConstructionProgress = 0.0f;
	CurrentHP = 1.0f;
}

void AAoABuilding::FinishConstruction()
{
	if (!HasAuthority()) return;
	BuildingState = EBuildingState::Complete;
	ConstructionProgress = 1.0f;
	CurrentHP = MaxHP;
}

void AAoABuilding::AddConstructionProgress(float DeltaProgress)
{
	if (!HasAuthority() || BuildingState != EBuildingState::UnderConstruction) return;
	ConstructionProgress += DeltaProgress / ConstructionTime;
	CurrentHP = FMath::Lerp(1.0f, MaxHP, ConstructionProgress);
	if (ConstructionProgress >= 1.0f)
	{
		FinishConstruction();
	}
}

void AAoABuilding::TrainUnit(TSubclassOf<AAoAUnit> UnitClass)
{
	if (!HasAuthority()) return;
	if (BuildingState != EBuildingState::Complete) return;

	// Check population and resources
	auto* GI = GetGameInstance();
	// In complete build: verify resources and pop cap before queueing
	TrainQueue.Add(UnitClass);

	if (CurrentTrainTime <= 0.0f && TrainQueue.Num() > 0)
	{
		CurrentTrainTime = 7.0f; // Base train time
		CurrentTrainProgress = 0.0f;
	}
}

void AAoABuilding::UpdateTraining(float DeltaTime)
{
	if (TrainQueue.IsEmpty()) return;

	CurrentTrainProgress += DeltaTime;
	if (CurrentTrainProgress >= CurrentTrainTime)
	{
		TSubclassOf<AAoAUnit> UnitClass = TrainQueue[0];
		TrainQueue.RemoveAt(0);
		SpawnTrainedUnit(UnitClass);

		CurrentTrainProgress = 0.0f;
		CurrentTrainTime = TrainQueue.IsEmpty() ? 0.0f : 7.0f;
	}
}

void AAoABuilding::SpawnTrainedUnit(TSubclassOf<AAoAUnit> UnitClass)
{
	FVector SpawnLoc = GetActorLocation() + FVector(200, 0, 0);
	FActorSpawnParameters SpawnParams;
	auto* NewUnit = GetWorld()->SpawnActor<AAoAUnit>(UnitClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	if (NewUnit)
	{
		NewUnit->SetOwnerPlayer(OwnerPlayerId);
		NewUnit->SetEmpireIndex(EmpireIndex);
		// Determine role based on unit class — simplified
		NewUnit->UnitRoleIndex = 0; // Would be set from data asset

		// Send to rally point if set
		if (bRallySet)
		{
			NewUnit->CommandMove(RallyPoint);
		}
	}
}

void AAoABuilding::SetRallyPoint(const FVector& Location)
{
	if (!HasAuthority()) return;
	RallyPoint = Location;
	bRallySet = true;
}

void AAoABuilding::ApplyDamage(float Damage)
{
	if (!HasAuthority()) return;
	CurrentHP -= Damage;
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		BuildingState = EBuildingState::Destroyed;
		SetLifeSpan(2.0f);
	}
}

void AAoABuilding::UpdateDefense(float DeltaTime)
{
	if (!bHasAttack) return;
	AttackTimer -= DeltaTime;
	if (AttackTimer <= 0.0f)
	{
		AActor* Enemy = FindEnemyInRadius();
		if (Enemy)
		{
			AttackTimer = 1.0f; // Attack cooldown
			if (auto* EnemyUnit = Cast<AAoAUnit>(Enemy))
				EnemyUnit->ApplyDamage(AttackDamage);
		}
	}
}

AActor* AAoABuilding::FindEnemyInRadius() const
{
	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() == OwnerPlayerId) continue;
		if (It->IsDead()) continue;
		float D = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (D <= AttackRange)
			return *It;
	}
	return nullptr;
}

