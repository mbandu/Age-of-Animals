#pragma once

// Stub types to replace excluded data asset headers
enum class EAnimalEmpire : unsigned char { Panda, Bunny, Monkey, Gorilla, Cat };
enum class EResourceType : unsigned char { Food, Wood, Stone, Gold };
enum class EUnitRole : unsigned char { Villager, Warrior, Archer, Special };
enum class EBuildingRole : unsigned char { TownCenter, House, Barracks, Tower };

struct FResourceCost { int Food=0; int Wood=0; int Stone=0; int Gold=0; };
struct FEmpireBonus { float HPMultiplier=1; float SpeedMultiplier=1; float AttackMultiplier=1; float RangeMultiplier=1; float GatherMultiplier=1; };
struct FAoALobbyPlayer { FString PlayerName; int EmpireIndex=0; bool bIsAI=false; bool bIsReady=false; };

class UAoAEmpireData;
class UAoAUnitData;
class UAoABuildingData;
