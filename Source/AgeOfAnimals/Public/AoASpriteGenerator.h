#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
class UAoASpriteGenerator : public UObject
{
public:
	static UTexture2D* GenerateEmpirePortrait(int32 EmpireIndex, int32 Size = 256);
	static UTexture2D* GenerateUnitSprite(int32 EmpireIndex, int32 UnitRole, int32 Frame, int32 Direction, int32 Size = 128);
	static UTexture2D* GenerateBuildingSprite(int32 EmpireIndex, int32 BuildingRole, int32 Size = 256);
	static UTexture2D* GenerateTerrainTexture(int32 TileType, int32 Size = 128);
	static void GenerateAllAssets();
};
