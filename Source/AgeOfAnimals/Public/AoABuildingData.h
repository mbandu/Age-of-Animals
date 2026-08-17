#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "AoAUnitData.h"

enum class EBuildingRole : uint8 { TownCenter, House, Barracks, Tower };

class UAoABuildingData : public UDataAsset
{
public:
	FText BuildingName;
	EBuildingRole Role = EBuildingRole::TownCenter;
	float MaxHP = 1200.0f;
	float AttackDamage = 0.0f;
	float AttackRange = 0.0f;
	float AttackCooldown = 1.0f;
	bool bHasAttack = false;
	int32 PopulationProvided = 0;
	float SightRange = 1000.0f;
	int32 FootprintSize = 3;
	FResourceCost Cost;
	float BuildTime = 15.0f;
	TArray<UObject*> TrainableUnits;
	TObjectPtr<UStaticMesh> BuildingMesh;
	TArray<TObjectPtr<UTexture2D>> ConstructionStages;
	TObjectPtr<UTexture2D> PortraitTexture;
};
