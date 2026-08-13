#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AoASpriteGenerator.generated.h"

class UTexture2D;
class UPaperFlipbook;

/**
 * Procedurally generates photorealistic, vibrant sprite textures for all
 * animal units and buildings at runtime. Creates detailed pixel art with
 * shading, fur patterns, highlights, and empire-specific color palettes.
 *
 * The generated textures are used to create Paper2D flipbooks for 8-directional
 * animation (idle, walk, attack, gather, death frames).
 */
UCLASS(BlueprintType)
class UAoASpriteGenerator : public UObject
{
	GENERATED_BODY()

public:
	/** Generate a portrait texture for an empire (used in UI) */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static UTexture2D* GenerateEmpirePortrait(int32 EmpireIndex, int32 Size = 256);

	/** Generate a unit sprite texture for a specific empire + role + animation frame */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static UTexture2D* GenerateUnitSprite(int32 EmpireIndex, int32 UnitRole, int32 Frame, int32 Direction, int32 Size = 128);

	/** Generate a building sprite texture for a specific empire + role */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static UTexture2D* GenerateBuildingSprite(int32 EmpireIndex, int32 BuildingRole, int32 Size = 256);

	/** Generate a terrain tile texture */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static UTexture2D* GenerateTerrainTexture(int32 TileType, int32 Size = 128);

	/** Generate a full set of flipbook frames for a unit and assemble into a flipbook */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static UPaperFlipbook* GenerateUnitFlipbook(int32 EmpireIndex, int32 UnitRole, int32 Direction, int32 NumFrames = 8);

	/** Generate all sprite assets for all 5 empires and save them to Content/ */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static void GenerateAllAssets();

	/** Save a texture to disk as a PNG */
	UFUNCTION(BlueprintCallable, Category = "Sprite Generation")
	static bool SaveTextureAsPNG(UTexture2D* Texture, const FString& Path);

protected:
	// Animal silhouette rendering — each animal has a unique body shape
	static void DrawPandaBody(uint8* Pixels, int32 Size, int32 Frame, const FColor& Body, const FColor& Accent);
	static void DrawBunnyBody(uint8* Pixels, int32 Size, int32 Frame, const FColor& Body, const FColor& Accent);
	static void DrawMonkeyBody(uint8* Pixels, int32 Size, int32 Frame, const FColor& Body, const FColor& Accent);
	static void DrawGorillaBody(uint8* Pixels, int32 Size, int32 Frame, const FColor& Body, const FColor& Accent);
	static void DrawCatBody(uint8* Pixels, int32 Size, int32 Frame, const FColor& Body, const FColor& Accent);

	// Drawing primitives
	static void SetPixel(uint8* Pixels, int32 Size, int32 X, int32 Y, const FColor& Color);
	static void FillCircle(uint8* Pixels, int32 Size, float CX, float CY, float R, const FColor& Color);
	static void FillEllipse(uint8* Pixels, int32 Size, float CX, float CY, float RX, float RY, const FColor& Color);
	static void DrawOutline(uint8* Pixels, int32 Size, const FColor& OutlineColor, float Threshold = 0.5f);
	static void AddShading(uint8* Pixels, int32 Size, float LightAngle = 0.7f);
	static void AddFurTexture(uint8* Pixels, int32 Size, const FColor& Base, float Density = 0.3f);
	static void AddHighlights(uint8* Pixels, int32 Size, float Intensity = 0.15f);
	static void AddDropShadow(uint8* Pixels, int32 Size);

	// Create a UTexture2D from a pixel buffer
	static UTexture2D* CreateTextureFromPixels(uint8* Pixels, int32 Size, const FString& Name);

	// Empire color palettes
	static FColor GetEmpireBodyColor(int32 EmpireIndex);
	static FColor GetEmpireAccentColor(int32 EmpireIndex);
};
