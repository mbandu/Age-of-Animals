#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AoAEmpireData.generated.h"

UENUM(BlueprintType)
enum class EAnimalEmpire : uint8
{
	Panda   UMETA(DisplayName = "Panda Empire"),
	Bunny   UMETA(DisplayName = "Bunny Empire"),
	Monkey  UMETA(DisplayName = "Monkey Empire"),
	Gorilla UMETA(DisplayName = "Gorilla Empire"),
	Cat     UMETA(DisplayName = "Cat Empire"),
};

// ---- Resource types ----
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Food  UMETA(DisplayName = "Food"),
	Wood  UMETA(DisplayName = "Wood"),
	Stone UMETA(DisplayName = "Stone"),
	Gold  UMETA(DisplayName = "Gold"),
};

USTRUCT(BlueprintType)
struct FEmpireBonus
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus")
	float HPMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus")
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus")
	float AttackMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus")
	float RangeMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus")
	float GatherMultiplier = 1.0f;
};

/**
 * Data asset that defines one animal empire: its visual theme, civilization
 * bonus, and references to its unit/building data assets.
 */
UCLASS(BlueprintType, Const)
class UAoAEmpireData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	EAnimalEmpire Empire = EAnimalEmpire::Panda;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	FText EmpireName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	FLinearColor PrimaryColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	FLinearColor AccentColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Empire")
	FEmpireBonus Bonus;

	/** Unit data assets for this empire — index matches EUnitRole */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Units")
	TArray<TObjectPtr<class UAoAUnitData>> Units;

	/** Building data assets for this empire — index matches EBuildingRole */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buildings")
	TArray<TObjectPtr<class UAoABuildingData>> Buildings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> SelectSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> AttackSound;

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AoAEmpire", *EmpireName.ToString());
	}
};

