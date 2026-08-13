#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AoAEmpireData.h"
#include "AoALevelEditorTool.generated.h"

class UToolMenu;

/**
 * Custom level editor tools for Age of Animals map creation.
 * Adds toolbar buttons for:
 *  - Terrain painting (grass, water, forest, bamboo, rock, dirt, sand, flowers)
 *  - Resource node placement (forest=wood, bamboo=food, rock=stone)
 *  - Spawn point marking
 *  - Map save/load (to .aoamap JSON format)
 *  - Map generation (procedural random maps)
 *  - Empire start position assignment
 */
UCLASS(BlueprintType)
class UAoALevelEditorTool : public UObject
{
	GENERATED_BODY()

public:
	/** Register level editor toolbar buttons and menu entries */
	static void RegisterToolbar();

	/** Terrain paint mode — paint tiles with a brush */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static void PaintTerrain(int32 TileType, const FVector& Location, float BrushRadius);

	/** Place a resource node at a location */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static void PlaceResourceNode(const FVector& Location, EResourceType Type, int32 Amount);

	/** Mark a spawn point for an empire */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static void MarkSpawnPoint(int32 EmpireIndex, const FVector& Location);

	/** Generate a random map with the given seed */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static void GenerateRandomMap(uint32 Seed, int32 MapSize);

	/** Save the current map to a .aoamap file */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static bool SaveMap(const FString& FilePath);

	/** Load a .aoamap file and rebuild the level */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static bool LoadMap(const FString& FilePath);

	/** Export the current level as a playable map asset */
	UFUNCTION(BlueprintCallable, Category = "Level Editor")
	static bool ExportAsMapAsset(const FString& AssetName);

protected:
	static void OnTerrainPaintClicked(UToolMenu* Menu);
	static void OnResourcePlaceClicked(UToolMenu* Menu);
	static void OnGenerateMapClicked(UToolMenu* Menu);
	static void OnSaveMapClicked(UToolMenu* Menu);
	static void OnLoadMapClicked(UToolMenu* Menu);

	/** Terrain brush state — tracks what tile type and brush size are active */
	static int32 CurrentTileType;
	static float CurrentBrushRadius;
	static bool bIsPainting;
};

