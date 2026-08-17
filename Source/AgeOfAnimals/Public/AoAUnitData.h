#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AoAEmpireData.h"
#include "Engine/Texture2D.h"
#include "AoAUnitData.generated.h"

UENUM(BlueprintType)
enum class EUnitRole : uint8
{
	Villager UMETA(DisplayName = "Villager / Worker"),
	Warrior  UMETA(DisplayName = "Warrior / Melee"),
	Archer   UMETA(DisplayName = "Archer / Ranged"),
	Special  UMETA(DisplayName = "Unique / Special"),
};

USTRUCT(BlueprintType)
struct FResourceCost
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	int32 Food = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	int32 Wood = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	int32 Stone = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	int32 Gold = 0;
};

/**
 * Data asset defining a unit type — stats, cost, sprite, and role.
 * Each empire has four of these (villager, warrior, archer, special).
 */
UCLASS(BlueprintType, Const)
class UAoAUnitData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
	FText UnitName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
	EUnitRole Role = EUnitRole::Villager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHP = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MoveSpeed = 300.0f; // cm/s

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackRange = 60.0f; // cm

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackCooldown = 1.0f; // seconds

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	bool bIsRanged = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float ProjectileSpeed = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 PopulationCost = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gathering")
	bool bCanGather = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gathering")
	float GatherRate = 1.0f; // resources per second

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gathering")
	int32 CarryCapacity = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gathering")
	bool bCanBuild = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gathering")
	float BuildPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	FResourceCost Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	float TrainTime = 7.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TArray<UObject*> IdleFlipbooks; // per facing direction

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TArray<UObject*> MoveFlipbooks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TArray<UObject*> AttackFlipbooks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TArray<UObject*> GatherFlipbooks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UTexture2D> PortraitTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TArray<TObjectPtr<USoundBase>> VoiceSounds;


};
