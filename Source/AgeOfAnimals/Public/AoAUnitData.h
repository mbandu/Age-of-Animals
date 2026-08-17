#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "AoAEmpireData.h"

enum class EUnitRole : uint8 { Villager, Warrior, Archer, Special };

struct FResourceCost { int32 Food=0; int32 Wood=0; int32 Stone=0; int32 Gold=0; };

class UAoAUnitData : public UDataAsset
{
public:
	FText UnitName;
	EUnitRole Role = EUnitRole::Villager;
	float MaxHP = 40.0f;
	float MoveSpeed = 300.0f;
	float AttackDamage = 6.0f;
	float AttackRange = 60.0f;
	float AttackCooldown = 1.0f;
	bool bIsRanged = false;
	float ProjectileSpeed = 800.0f;
	int32 PopulationCost = 1;
	bool bCanGather = false;
	float GatherRate = 1.0f;
	int32 CarryCapacity = 10;
	bool bCanBuild = false;
	float BuildPower = 1.0f;
	FResourceCost Cost;
	float TrainTime = 7.0f;
	TArray<UObject*> IdleFlipbooks;
	TArray<UObject*> MoveFlipbooks;
	TArray<UObject*> AttackFlipbooks;
	TArray<UObject*> GatherFlipbooks;
	TObjectPtr<UTexture2D> PortraitTexture;
	TArray<TObjectPtr<USoundBase>> VoiceSounds;
};
