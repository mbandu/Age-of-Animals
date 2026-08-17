#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AoAUnitData.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "AoABuildingData.generated.h"

UENUM(BlueprintType)
enum class EBuildingRole : uint8
{
	TownCenter UMETA(DisplayName = "Town Center / Den"),
	House      UMETA(DisplayName = "House / Hut"),
	Barracks   UMETA(DisplayName = "Barracks / Camp"),
	Tower      UMETA(DisplayName = "Tower / Defense"),
};

/**
 * Data asset defining a building type.
 */
UCLASS(BlueprintType, Const)
class UAoABuildingData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	FText BuildingName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	EBuildingRole Role = EBuildingRole::TownCenter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHP = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackRange = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	bool bHasAttack = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 PopulationProvided = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float SightRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	int32 FootprintSize = 3; // tiles

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	FResourceCost Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	float BuildTime = 15.0f;

	/** Unit composite indices this building can train */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Production")
	TArray<UObject*> TrainableUnits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMesh> BuildingMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TArray<TObjectPtr<UTexture2D>> ConstructionStages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UTexture2D> PortraitTexture;


};

