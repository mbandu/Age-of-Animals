#include "AoAPlayerState.h"
#include "Net/UnrealNetwork.h"

AAoAPlayerState::AAoAPlayerState()
{
	bReplicates = true;
}

void AAoAPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAoAPlayerState, EmpireIndex);
	DOREPLIFETIME(AAoAPlayerState, bIsAI);
	DOREPLIFETIME(AAoAPlayerState, Food);
	DOREPLIFETIME(AAoAPlayerState, Wood);
	DOREPLIFETIME(AAoAPlayerState, Stone);
	DOREPLIFETIME(AAoAPlayerState, Gold);
	DOREPLIFETIME(AAoAPlayerState, PopulationUsed);
	DOREPLIFETIME(AAoAPlayerState, PopulationCap);
	DOREPLIFETIME(AAoAPlayerState, bIsAlive);
	DOREPLIFETIME(AAoAPlayerState, CurrentAge);
	DOREPLIFETIME(AAoAPlayerState, Kills);
}

bool AAoAPlayerState::SpendResources(int32 InFood, int32 InWood, int32 InStone, int32 InGold)
{
	if (!CanAfford(InFood, InWood, InStone, InGold)) return false;
	Food -= InFood;
	Wood -= InWood;
	Stone -= InStone;
	Gold -= InGold;
	return true;
}

void AAoAPlayerState::AddResource(EResourceType Type, int32 Amount)
{
	switch (Type)
	{
	case EResourceType::Food:  Food += Amount; break;
	case EResourceType::Wood:  Wood += Amount; break;
	case EResourceType::Stone: Stone += Amount; break;
	case EResourceType::Gold:  Gold += Amount; break;
	}
}

bool AAoAPlayerState::CanAfford(int32 InFood, int32 InWood, int32 InStone, int32 InGold) const
{
	return Food >= InFood && Wood >= InWood && Stone >= InStone && Gold >= InGold;
}
