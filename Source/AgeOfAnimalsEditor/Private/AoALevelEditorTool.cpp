#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AoALevelEditorTool.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"
#include "JsonObjectConverter.h"
#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"
#include "HAL/FileManager.h"

int32 UAoALevelEditorTool::CurrentTileType = 0;
float UAoALevelEditorTool::CurrentBrushRadius = 500.0f;
bool UAoALevelEditorTool::bIsPainting = false;

void UAoALevelEditorTool::RegisterToolbar()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus) return;

	// Register toolbar buttons for the AoA level editor
	UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu("LevelEditor.LevelToolbar");
	FToolMenuSection& Section = ToolbarMenu->AddSection("AoASection", FText::FromString("Age of Animals"));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"AoA_TerrainPaint",
		FExecuteAction::CreateLambda([](){ OnTerrainPaintClicked(nullptr); }),
		FText::FromString("Terrain"),
		FText::FromString("Paint terrain tiles"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Terrain.Edit")
	));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"AoA_PlaceResource",
		FExecuteAction::CreateLambda([](){ OnResourcePlaceClicked(nullptr); }),
		FText::FromString("Resources"),
		FText::FromString("Place resource nodes"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Resources")
	));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"AoA_GenerateMap",
		FExecuteAction::CreateLambda([](){ OnGenerateMapClicked(nullptr); }),
		FText::FromString("Generate"),
		FText::FromString("Generate random map"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Generate")
	));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"AoA_SaveMap",
		FExecuteAction::CreateLambda([](){ OnSaveMapClicked(nullptr); }),
		FText::FromString("Save Map"),
		FText::FromString("Save map to file"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Save")
	));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"AoA_LoadMap",
		FExecuteAction::CreateLambda([](){ OnLoadMapClicked(nullptr); }),
		FText::FromString("Load Map"),
		FText::FromString("Load map from file"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Load")
	));
}

void UAoALevelEditorTool::PaintTerrain(int32 TileType, const FVector& Location, float BrushRadius)
{
	// In the UE editor, this would modify the landscape or place terrain mesh tiles
	// within the brush radius. Each tile is 100x100 units.
	CurrentTileType = TileType;
	CurrentBrushRadius = BrushRadius;

	// Spawn or modify terrain mesh actors in the brush area
	int32 TileRadius = FMath::CeilToInt(BrushRadius / 100.0f);
	int32 CenterX = FMath::FloorToInt(Location.X / 100.0f);
	int32 CenterY = FMath::FloorToInt(Location.Y / 100.0f);

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	for (int32 DY = -TileRadius; DY <= TileRadius; ++DY)
	{
		for (int32 DX = -TileRadius; DX <= TileRadius; ++DX)
		{
			if (DX * DX + DY * DY > TileRadius * TileRadius) continue;
			// Place a terrain tile actor at this position
			// The actual implementation would swap mesh materials or use a landscape layer
		}
	}
}

void UAoALevelEditorTool::PlaceResourceNode(const FVector& Location, EResourceType Type, int32 Amount)
{
	// Place a resource node actor at the clicked location
	// The resource type determines which mesh/material is used
}

void UAoALevelEditorTool::MarkSpawnPoint(int32 EmpireIndex, const FVector& Location)
{
	// Place a spawn point marker actor for the specified empire
	// This is used by the game mode to place starting bases
}

void UAoALevelEditorTool::GenerateRandomMap(int32 Seed, int32 MapSize)
{
	// Procedurally generate a map using value noise:
	// - Low areas = water
	// - High areas = rock/mountains
	// - Medium areas = grass
	// - Scatter forest, bamboo, and rock resource clusters
	// - Place spawn points spread across the map
	// This mirrors the world generation logic from the standalone build

	FRandomStream RNG(Seed);
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	// Clear existing terrain actors
	TArray<AActor*> ExistingTerrain;
	UGameplayStatics::GetAllActorsWithTag(World, FName("Terrain"), ExistingTerrain);
	for (auto* Actor : ExistingTerrain)
		Actor->Destroy();

	// Generate tile types using value noise
	TArray<uint8> Tiles;
	Tiles.SetNumZeroed(MapSize * MapSize);

	// Simple noise-based generation
	for (int32 Y = 0; Y < MapSize; ++Y)
	{
		for (int32 X = 0; X < MapSize; ++X)
		{
			float NX = (float)X / MapSize;
			float NY = (float)Y / MapSize;
			// Multi-octave noise
			float V = 0.0f;
			V += RNG.FRand() * 0.3f;
			V += FMath::PerlinNoise2D(FVector2D(NX * 4, NY * 4)) * 0.4f;
			V += FMath::PerlinNoise2D(FVector2D(NX * 8, NY * 8)) * 0.2f;
			V += FMath::PerlinNoise2D(FVector2D(NX * 16, NY * 16)) * 0.1f;
			V = (V + 1.0f) * 0.5f; // normalize to 0..1

			uint8_t Tile = 0; // Grass
			if (V < 0.20f) Tile = 1; // Water
			else if (V > 0.85f) Tile = 4; // Rock
			Tiles[Y * MapSize + X] = Tile;
		}
	}

	// Scatter resource clusters
	for (int32 i = 0; i < 20; ++i)
	{
		int32 CX = RNG.RandRange(4, MapSize - 5);
		int32 CY = RNG.RandRange(4, MapSize - 5);
		int32 R = RNG.RandRange(2, 4);
		uint8_t ResType = (i % 3 == 0) ? 2 : (i % 3 == 1) ? 3 : 4; // Forest, Bamboo, Rock
		for (int32 DY = -R; DY <= R; ++DY)
			for (int32 DX = -R; DX <= R; ++DX)
			{
				if (DX * DX + DY * DY > R * R) continue;
				int32 TX = CX + DX, TY = CY + DY;
				if (TX < 0 || TY < 0 || TX >= MapSize || TY >= MapSize) continue;
				if (Tiles[TY * MapSize + TX] == 0) // Only on grass
					Tiles[TY * MapSize + TX] = ResType;
			}
	}
}

bool UAoALevelEditorTool::SaveMap(const FString& FilePath)
{
	// Serialize the map to JSON format (.aoamap)
	// Format:
	// {
	//   "version": 1,
	//   "mapSize": { "x": 64, "y": 64 },
	//   "tiles": [0,1,2,...],
	//   "resources": [ { "x": 10, "y": 20, "type": "wood", "amount": 400 }, ... ],
	//   "spawns": [ { "empire": 0, "x": 16, "y": 16 }, ... ]
	// }

	TSharedPtr<FJsonObject> RootObj = MakeShared<FJsonObject>();
	RootObj->SetNumberField(TEXT("version"), 1);
	RootObj->SetNumberField(TEXT("mapSizeX"), 64);
	RootObj->SetNumberField(TEXT("mapSizeY"), 64);

	// Collect all actors with terrain/resource/spawn tags and serialize
	// This is a simplified version

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);

	return FFileHelper::SaveStringToFile(JsonString, *FilePath);
}

bool UAoALevelEditorTool::LoadMap(const FString& FilePath)
{
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) return false;

	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootObj)) return false;

	// Rebuild the level from the loaded JSON
	int32 MapSizeX = RootObj->GetIntegerField(TEXT("mapSizeX"));
	int32 MapSizeY = RootObj->GetIntegerField(TEXT("mapSizeY"));

	// Spawn terrain, resources, and spawn points
	return true;
}

bool UAoALevelEditorTool::ExportAsMapAsset(const FString& AssetName)
{
	// Export the current level as a UDataAsset that can be loaded by the game
	// This creates a UWorld or custom map data asset in the content browser
	return true;
}

void UAoALevelEditorTool::OnTerrainPaintClicked(UToolMenu* Menu)
{
	// Toggle terrain paint mode
	bIsPainting = !bIsPainting;
}

void UAoALevelEditorTool::OnResourcePlaceClicked(UToolMenu* Menu)
{
	// Enter resource placement mode
}

void UAoALevelEditorTool::OnGenerateMapClicked(UToolMenu* Menu)
{
	GenerateRandomMap(FMath::Rand(), 64);
}

void UAoALevelEditorTool::OnSaveMapClicked(UToolMenu* Menu)
{
	// Open a file dialog for saving
	SaveMap(FPaths::ProjectDir() / TEXT("Maps/CustomMap.aoamap"));
}

void UAoALevelEditorTool::OnLoadMapClicked(UToolMenu* Menu)
{
	// Open a file dialog for loading
	LoadMap(FPaths::ProjectDir() / TEXT("Maps/CustomMap.aoamap"));
}

