#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AoAEmpireData.h"
class UAoALevelEditorTool : public UObject
{
public:
	static void PaintTerrain(int32 TileType, const FVector& Location, float BrushRadius);
	static void GenerateRandomMap(int32 Seed, int32 MapSize);
	static bool SaveMap(const FString& FilePath);
	static bool LoadMap(const FString& FilePath);
};
