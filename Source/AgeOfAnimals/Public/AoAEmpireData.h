#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"

enum class EAnimalEmpire : uint8 { Panda, Bunny, Monkey, Gorilla, Cat };
enum class EResourceType : uint8 { Food, Wood, Stone, Gold };

struct FEmpireBonus { float HPMultiplier=1; float SpeedMultiplier=1; float AttackMultiplier=1; float RangeMultiplier=1; float GatherMultiplier=1; };

class UAoAEmpireData : public UDataAsset
{
public:
	EAnimalEmpire Empire = EAnimalEmpire::Panda;
	FText EmpireName;
	FText Description;
	FLinearColor PrimaryColor = FLinearColor::White;
	FLinearColor AccentColor = FLinearColor::Black;
	FEmpireBonus Bonus;
	TArray<UObject*> Units;
	TArray<UObject*> Buildings;
	TObjectPtr<USoundBase> SelectSound;
	TObjectPtr<USoundBase> AttackSound;
};
