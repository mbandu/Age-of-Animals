#include <cstring>
#include "AoASpriteGenerator.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "Paper2D/Classes/PaperFlipbook.h"
#include "Paper2D/Classes/PaperFlipbookFrame.h"
#include "Paper2D/Classes/PaperSprite.h"
#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetUtilities.h"
#include "Package.h"
#include "ObjectTools.h"
#include "Factories/TextureFactory.h"
#include "Math/UnrealMathUtility.h"
#include <cmath>

// ============================================================================
//  Drawing primitives — operate on raw RGBA pixel buffers
// ============================================================================

void UAoASpriteGenerator::SetPixel(uint8* P, int32 S, int32 X, int32 Y, const FColor& C)
{
	if (X < 0 || Y < 0 || X >= S || Y >= S) return;
	int32 Idx = (Y * S + X) * 4;
	P[Idx]     = C.R;
	P[Idx + 1] = C.G;
	P[Idx + 2] = C.B;
	P[Idx + 3] = C.A;
}

void UAoASpriteGenerator::FillCircle(uint8* P, int32 S, float CX, float CY, float R, const FColor& C)
{
	int32 MinX = FMath::Max(0, (int32)(CX - R - 1));
	int32 MaxX = FMath::Min(S - 1, (int32)(CX + R + 1));
	int32 MinY = FMath::Max(0, (int32)(CY - R - 1));
	int32 MaxY = FMath::Min(S - 1, (int32)(CY + R + 1));
	for (int32 Y = MinY; Y <= MaxY; ++Y)
		for (int32 X = MinX; X <= MaxX; ++X)
		{
			float DX = X - CX, DY = Y - CY;
			if (DX * DX + DY * DY <= R * R)
				SetPixel(P, S, X, Y, C);
		}
}

void UAoASpriteGenerator::FillEllipse(uint8* P, int32 S, float CX, float CY, float RX, float RY, const FColor& C)
{
	int32 MinX = FMath::Max(0, (int32)(CX - RX - 1));
	int32 MaxX = FMath::Min(S - 1, (int32)(CX + RX + 1));
	int32 MinY = FMath::Max(0, (int32)(CY - RY - 1));
	int32 MaxY = FMath::Min(S - 1, (int32)(CY + RY + 1));
	for (int32 Y = MinY; Y <= MaxY; ++Y)
		for (int32 X = MinX; X <= MaxX; ++X)
		{
			float DX = (X - CX) / RX, DY = (Y - CY) / RY;
			if (DX * DX + DY * DY <= 1.0f)
				SetPixel(P, S, X, Y, C);
		}
}

void UAoASpriteGenerator::DrawOutline(uint8* P, int32 S, const FColor& Outline, float Threshold)
{
	uint8* Temp = new uint8[S * S * 4];
	std::memcpy(Temp, P, S * S * 4);
	for (int32 Y = 1; Y < S - 1; ++Y)
		for (int32 X = 1; X < S - 1; ++X)
		{
			int32 Idx = (Y * S + X) * 4;
			if (Temp[Idx + 3] < 128) continue;
			// Check neighbors — if any is transparent, draw outline
			bool bEdge = false;
			for (int dy = -1; dy <= 1 && !bEdge; ++dy)
				for (int dx = -1; dx <= 1; ++dx)
				{
					int32 NIdx = ((Y + dy) * S + (X + dx)) * 4;
					if (Temp[NIdx + 3] < 128) { bEdge = true; break; }
				}
			if (bEdge)
				SetPixel(P, S, X, Y, Outline);
		}
	delete[] Temp;
}

void UAoASpriteGenerator::AddShading(uint8* P, int32 S, float LightAngle)
{
	// Apply directional lighting — brighter on the light side, darker on the opposite
	float LX = FMath::Cos(LightAngle), LY = FMath::Sin(LightAngle);
	for (int32 Y = 0; Y < S; ++Y)
		for (int32 X = 0; X < S; ++X)
		{
			int32 Idx = (Y * S + X) * 4;
			if (P[Idx + 3] < 10) continue;
			float CX = S * 0.5f, CY = S * 0.5f;
			float DX = (X - CX) / (S * 0.5f), DY = (Y - CY) / (S * 0.5f);
			float Dot = DX * LX + DY * LY;
			float Shade = 1.0f + Dot * 0.25f;
			Shade = FMath::Clamp(Shade, 0.5f, 1.3f);
			P[Idx]     = (uint8)FMath::Clamp((int32)(P[Idx] * Shade), 0, 255);
			P[Idx + 1] = (uint8)FMath::Clamp((int32)(P[Idx + 1] * Shade), 0, 255);
			P[Idx + 2] = (uint8)FMath::Clamp((int32)(P[Idx + 2] * Shade), 0, 255);
		}
}

void UAoASpriteGenerator::AddFurTexture(uint8* P, int32 S, const FColor& Base, float Density)
{
	// Add noise-based fur texture detail
	FRandomStream RNG(12345);
	for (int32 Y = 0; Y < S; ++Y)
		for (int32 X = 0; X < S; ++X)
		{
			int32 Idx = (Y * S + X) * 4;
			if (P[Idx + 3] < 10) continue;
			float N = RNG.FRand();
			if (N < Density)
			{
				int32 Variation = (int32)(N * 60.0f) - 30;
				P[Idx]     = (uint8)FMath::Clamp(P[Idx] + Variation, 0, 255);
				P[Idx + 1] = (uint8)FMath::Clamp(P[Idx + 1] + Variation, 0, 255);
				P[Idx + 2] = (uint8)FMath::Clamp(P[Idx + 2] + Variation, 0, 255);
			}
		}
}

void UAoASpriteGenerator::AddHighlights(uint8* P, int32 S, float Intensity)
{
	// Add specular highlights — bright spots on the upper-left of shapes
	for (int32 Y = 0; Y < S; ++Y)
		for (int32 X = 0; X < S; ++X)
		{
			int32 Idx = (Y * S + X) * 4;
			if (P[Idx + 3] < 10) continue;
			float CX = S * 0.35f, CY = S * 0.35f;
			float DX = X - CX, DY = Y - CY;
			float D = FMath::Sqrt(DX * DX + DY * DY);
			if (D < S * 0.15f)
			{
				float H = (1.0f - D / (S * 0.15f)) * Intensity;
				P[Idx]     = (uint8)FMath::Clamp(P[Idx] + (int32)(255 * H), 0, 255);
				P[Idx + 1] = (uint8)FMath::Clamp(P[Idx + 1] + (int32)(255 * H), 0, 255);
				P[Idx + 2] = (uint8)FMath::Clamp(P[Idx + 2] + (int32)(255 * H), 0, 255);
			}
		}
}

void UAoASpriteGenerator::AddDropShadow(uint8* P, int32 S)
{
	// Add a soft drop shadow beneath the sprite
	uint8* Temp = new uint8[S * S * 4];
	std::memcpy(Temp, P, S * S * 4);
	for (int32 Y = 0; Y < S; ++Y)
		for (int32 X = 0; X < S; ++X)
		{
			// Shadow offset down-right
			int32 SX = X + (int32)(S * 0.04f);
			int32 SY = Y + (int32)(S * 0.06f);
			if (SX >= S || SY >= S) continue;
			int32 SIdx = (SY * S + SX) * 4;
			if (Temp[SIdx + 3] > 128)
			{
				int32 Idx = (Y * S + X) * 4;
				if (P[Idx + 3] < 10)
				{
					P[Idx]     = 20;
					P[Idx + 1] = 20;
					P[Idx + 2] = 30;
					P[Idx + 3] = 80;
				}
			}
		}
	delete[] Temp;
}

FColor UAoASpriteGenerator::GetEmpireBodyColor(int32 E)
{
	static const FColor Colors[] = {
		FColor(240, 240, 235, 255),  // Panda — white/cream
		FColor(245, 240, 235, 255),  // Bunny — white
		FColor(180, 130, 80, 255),   // Monkey — brown
		FColor(100, 90, 85, 255),    // Gorilla — dark grey
		FColor(220, 190, 140, 255),  // Cat — orange/tan
	};
	return Colors[FMath::Clamp(E, 0, 4)];
}

FColor UAoASpriteGenerator::GetEmpireAccentColor(int32 E)
{
	static const FColor Colors[] = {
		FColor(30, 30, 30, 255),     // Panda — black patches
		FColor(200, 150, 120, 255),  // Bunny — pink/tan
		FColor(120, 80, 50, 255),    // Monkey — dark brown
		FColor(140, 120, 110, 255),  // Gorilla — grey
		FColor(40, 30, 25, 255),     // Cat — dark stripes
	};
	return Colors[FMath::Clamp(E, 0, 4)];
}

UTexture2D* UAoASpriteGenerator::CreateTextureFromPixels(uint8* Pixels, int32 Size, const FString& Name)
{
	UTexture2D* Tex = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8);
	if (!Tex) return nullptr;
	Tex->Name = *Name;
	Tex->SRGB = true;
	Tex->UpdateResource();

	FTexture2DMipMap& Mip = Tex->GetPlatformMips()[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, Pixels, Size * Size * 4);
	Mip.BulkData.Unlock();
	Tex->UpdateResource();

	return Tex;
}

// ============================================================================
//  Animal body drawing — each animal has a unique silhouette
// ============================================================================

void UAoASpriteGenerator::DrawPandaBody(uint8* P, int32 S, int32 Frame, const FColor& Body, const FColor& Accent)
{
	float Bob = FMath::Sin(Frame * 0.5f) * 2.0f; // idle bob
	float WalkOffset = FMath::Sin(Frame * 0.8f) * 3.0f;

	// Body — round, plump
	FillEllipse(P, S, S * 0.5f, S * 0.58f + Bob, S * 0.22f, S * 0.20f, Body);

	// Head — large round
	FillCircle(P, S, S * 0.5f, S * 0.32f + Bob, S * 0.18f, Body);

	// Black ears
	FillCircle(P, S, S * 0.38f, S * 0.20f + Bob, S * 0.06f, Accent);
	FillCircle(P, S, S * 0.62f, S * 0.20f + Bob, S * 0.06f, Accent);

	// Black eye patches
	FillEllipse(P, S, S * 0.43f, S * 0.33f + Bob, S * 0.05f, S * 0.07f, Accent);
	FillEllipse(P, S, S * 0.57f, S * 0.33f + Bob, S * 0.05f, S * 0.07f, Accent);

	// Eyes
	FillCircle(P, S, S * 0.44f, S * 0.33f + Bob, S * 0.018f, FColor::Black);
	FillCircle(P, S, S * 0.56f, S * 0.33f + Bob, S * 0.018f, FColor::Black);

	// Nose
	FillCircle(P, S, S * 0.50f, S * 0.38f + Bob, S * 0.022f, FColor(40, 40, 40, 255));

	// Black arms/legs
	FillEllipse(P, S, S * 0.32f, S * 0.62f + Bob, S * 0.06f, S * 0.10f, Accent);
	FillEllipse(P, S, S * 0.68f, S * 0.62f + Bob, S * 0.06f, S * 0.10f, Accent);
	FillEllipse(P, S, S * 0.38f, S * 0.76f + WalkOffset, S * 0.07f, S * 0.05f, Accent);
	FillEllipse(P, S, S * 0.62f, S * 0.76f - WalkOffset, S * 0.07f, S * 0.05f, Accent);
}

void UAoASpriteGenerator::DrawBunnyBody(uint8* P, int32 S, int32 Frame, const FColor& Body, const FColor& Accent)
{
	float Hop = FMath::Abs(FMath::Sin(Frame * 0.6f)) * 4.0f;
	float EarBend = FMath::Sin(Frame * 0.4f) * 0.05f;

	// Body — compact, egg-shaped
	FillEllipse(P, S, S * 0.5f, S * 0.62f - Hop, S * 0.16f, S * 0.18f, Body);

	// Head
	FillCircle(P, S, S * 0.5f, S * 0.38f - Hop, S * 0.13f, Body);

	// Long ears
	FillEllipse(P, S, S * 0.42f + EarBend * S, S * 0.18f - Hop, S * 0.04f, S * 0.13f, Body);
	FillEllipse(P, S, S * 0.58f - EarBend * S, S * 0.18f - Hop, S * 0.04f, S * 0.13f, Body);
	// Inner ears — pink
	FillEllipse(P, S, S * 0.42f + EarBend * S, S * 0.20f - Hop, S * 0.02f, S * 0.08f, FColor(220, 150, 160, 255));
	FillEllipse(P, S, S * 0.58f - EarBend * S, S * 0.20f - Hop, S * 0.02f, S * 0.08f, FColor(220, 150, 160, 255));

	// Eyes
	FillCircle(P, S, S * 0.44f, S * 0.38f - Hop, S * 0.015f, FColor(80, 60, 40, 255));
	FillCircle(P, S, S * 0.56f, S * 0.38f - Hop, S * 0.015f, FColor(80, 60, 40, 255));

	// Nose — small pink
	FillCircle(P, S, S * 0.50f, S * 0.42f - Hop, S * 0.015f, FColor(200, 120, 130, 255));

	// Feet
	FillEllipse(P, S, S * 0.38f, S * 0.78f - Hop, S * 0.06f, S * 0.04f, Body);
	FillEllipse(P, S, S * 0.62f, S * 0.78f - Hop, S * 0.06f, S * 0.04f, Body);

	// Tail — fluffy white
	FillCircle(P, S, S * 0.68f, S * 0.62f - Hop, S * 0.04f, FColor(255, 255, 255, 255));
}

void UAoASpriteGenerator::DrawMonkeyBody(uint8* P, int32 S, int32 Frame, const FColor& Body, const FColor& Accent)
{
	float Sway = FMath::Sin(Frame * 0.5f) * 2.0f;

	// Body — leaner
	FillEllipse(P, S, S * 0.5f, S * 0.58f + Sway, S * 0.15f, S * 0.20f, Body);

	// Head — round with lighter face
	FillCircle(P, S, S * 0.5f, S * 0.34f + Sway, S * 0.16f, Body);
	FillEllipse(P, S, S * 0.5f, S * 0.36f + Sway, S * 0.12f, S * 0.10f, FColor(210, 190, 160, 255)); // face

	// Ears — round, on sides
	FillCircle(P, S, S * 0.36f, S * 0.34f + Sway, S * 0.05f, Body);
	FillCircle(P, S, S * 0.64f, S * 0.34f + Sway, S * 0.05f, Body);

	// Eyes
	FillCircle(P, S, S * 0.45f, S * 0.33f + Sway, S * 0.02f, FColor(60, 40, 20, 255));
	FillCircle(P, S, S * 0.55f, S * 0.33f + Sway, S * 0.02f, FColor(60, 40, 20, 255));

	// Nostrils
	SetPixel(P, S, S * 0.49f, S * 0.40f + Sway, FColor(60, 40, 20, 255));
	SetPixel(P, S, S * 0.51f, S * 0.40f + Sway, FColor(60, 40, 20, 255));

	// Arms — long, reaching
	FillEllipse(P, S, S * 0.30f, S * 0.55f + Sway, S * 0.04f, S * 0.12f, Body);
	FillEllipse(P, S, S * 0.70f, S * 0.55f + Sway, S * 0.04f, S * 0.12f, Body);

	// Legs
	FillEllipse(P, S, S * 0.40f, S * 0.76f, S * 0.05f, S * 0.08f, Body);
	FillEllipse(P, S, S * 0.60f, S * 0.76f, S * 0.05f, S * 0.08f, Body);

	// Tail — curved
	for (int i = 0; i < 20; ++i)
	{
		float T = (float)i / 20.0f;
		float TX = S * 0.65f + T * S * 0.10f;
		float TY = S * 0.60f - T * S * 0.15f + FMath::Sin(T * 3.0f + Frame * 0.3f) * 4.0f;
		FillCircle(P, S, TX, TY, S * 0.02f * (1.0f - T * 0.5f), Body);
	}
}

void UAoASpriteGenerator::DrawGorillaBody(uint8* P, int32 S, int32 Frame, const FColor& Body, const FColor& Accent)
{
	float Breath = FMath::Sin(Frame * 0.3f) * 1.5f;

	// Body — massive, broad chest
	FillEllipse(P, S, S * 0.5f, S * 0.60f + Breath, S * 0.25f, S * 0.22f, Body);

	// Silverback accent on back
	FillEllipse(P, S, S * 0.5f, S * 0.55f + Breath, S * 0.18f, S * 0.06f, Accent);

	// Head — large, domed
	FillEllipse(P, S, S * 0.5f, S * 0.30f + Breath, S * 0.15f, S * 0.13f, Body);

	// Brow ridge
	FillEllipse(P, S, S * 0.5f, S * 0.27f + Breath, S * 0.14f, S * 0.04f, Accent);

	// Face — darker
	FillEllipse(P, S, S * 0.5f, S * 0.33f + Breath, S * 0.10f, S * 0.07f, FColor(60, 50, 45, 255));

	// Eyes
	FillCircle(P, S, S * 0.45f, S * 0.31f + Breath, S * 0.018f, FColor(180, 140, 100, 255));
	FillCircle(P, S, S * 0.55f, S * 0.31f + Breath, S * 0.018f, FColor(180, 140, 100, 255));

	// Nostrils
	SetPixel(P, S, S * 0.48f, S * 0.36f + Breath, FColor::Black);
	SetPixel(P, S, S * 0.52f, S * 0.36f + Breath, FColor::Black);

	// Massive arms
	FillEllipse(P, S, S * 0.25f, S * 0.58f + Breath, S * 0.07f, S * 0.15f, Body);
	FillEllipse(P, S, S * 0.75f, S * 0.58f + Breath, S * 0.07f, S * 0.15f, Body);

	// Knuckles on ground
	FillCircle(P, S, S * 0.22f, S * 0.78f, S * 0.04f, Accent);
	FillCircle(P, S, S * 0.78f, S * 0.78f, S * 0.04f, Accent);

	// Legs — thick
	FillEllipse(P, S, S * 0.38f, S * 0.80f, S * 0.08f, S * 0.06f, Body);
	FillEllipse(P, S, S * 0.62f, S * 0.80f, S * 0.08f, S * 0.06f, Body);
}

void UAoASpriteGenerator::DrawCatBody(uint8* P, int32 S, int32 Frame, const FColor& Body, const FColor& Accent)
{
	float Prowl = FMath::Sin(Frame * 0.7f) * 2.0f;
	float TailWag = FMath::Sin(Frame * 0.5f) * 0.15f;

	// Body — sleek, elongated
	FillEllipse(P, S, S * 0.5f, S * 0.58f + Prowl, S * 0.20f, S * 0.12f, Body);

	// Head — triangular
	FillCircle(P, S, S * 0.5f, S * 0.35f + Prowl, S * 0.12f, Body);

	// Pointed ears
	for (int i = 0; i < 8; ++i)
	{
		float T = (float)i / 8.0f;
		FillCircle(P, S, S * 0.40f - T * S * 0.02f, S * 0.25f + T * S * 0.02f + Prowl, S * 0.02f * (1.0f - T * 0.5f), Body);
		FillCircle(P, S, S * 0.60f + T * S * 0.02f, S * 0.25f + T * S * 0.02f + Prowl, S * 0.02f * (1.0f - T * 0.5f), Body);
	}
	// Inner ears
	FillEllipse(P, S, S * 0.40f, S * 0.27f + Prowl, S * 0.015f, S * 0.03f, FColor(180, 130, 120, 255));
	FillEllipse(P, S, S * 0.60f, S * 0.27f + Prowl, S * 0.015f, S * 0.03f, FColor(180, 130, 120, 255));

	// Stripes
	for (int i = 0; i < 5; ++i)
	{
		float Y = S * 0.50f + i * S * 0.04f + Prowl;
		FillEllipse(P, S, S * 0.5f, Y, S * 0.18f, S * 0.015f, Accent);
	}

	// Eyes — green, almond-shaped
	FillEllipse(P, S, S * 0.44f, S * 0.34f + Prowl, S * 0.025f, S * 0.015f, FColor(80, 160, 80, 255));
	FillEllipse(P, S, S * 0.56f, S * 0.34f + Prowl, S * 0.025f, S * 0.015f, FColor(80, 160, 80, 255));
	// Pupils
	SetPixel(P, S, S * 0.44f, S * 0.34f + Prowl, FColor::Black);
	SetPixel(P, S, S * 0.56f, S * 0.34f + Prowl, FColor::Black);

	// Nose — pink
	FillCircle(P, S, S * 0.50f, S * 0.40f + Prowl, S * 0.012f, FColor(200, 100, 110, 255));

	// Whiskers
	for (int i = 0; i < 3; ++i)
	{
		float Y = S * 0.40f + i * S * 0.02f + Prowl;
		SetPixel(P, S, S * 0.38f, Y, FColor(200, 200, 200, 255));
		SetPixel(P, S, S * 0.37f, Y, FColor(200, 200, 200, 255));
		SetPixel(P, S, S * 0.62f, Y, FColor(200, 200, 200, 255));
		SetPixel(P, S, S * 0.63f, Y, FColor(200, 200, 200, 255));
	}

	// Tail — long, curving
	for (int i = 0; i < 25; ++i)
	{
		float T = (float)i / 25.0f;
		float TX = S * 0.68f + T * S * 0.12f;
		float TY = S * 0.55f - T * S * 0.20f + FMath::Sin(T * 4.0f + TailWag * 10.0f) * 6.0f + Prowl;
		FillCircle(P, S, TX, TY, S * 0.025f * (1.0f - T * 0.4f), Body);
	}

	// Legs — slender
	FillEllipse(P, S, S * 0.38f, S * 0.74f + Prowl, S * 0.03f, S * 0.06f, Body);
	FillEllipse(P, S, S * 0.62f, S * 0.74f - Prowl, S * 0.03f, S * 0.06f, Body);
}

// ============================================================================
//  Public generation functions
// ============================================================================

UTexture2D* UAoASpriteGenerator::GenerateUnitSprite(int32 Empire, int32 Role, int32 Frame, int32 Dir, int32 Size)
{
	uint8* P = new uint8[Size * Size * 4];
	FMemory::Memset(P, 0, Size * Size * 4);

	FColor Body = GetEmpireBodyColor(Empire);
	FColor Accent = GetEmpireAccentColor(Empire);

	// Adjust body color based on unit role (warriors darker, villagers lighter, specials more vibrant)
	if (Role == 1) { Body = FColor(Body.R * 0.7f, Body.G * 0.7f, Body.B * 0.7f, 255); } // Warrior — darker
	if (Role == 2) { Body = FColor(Body.R * 0.85f, Body.G * 0.9f, Body.B * 0.75f, 255); } // Archer — tinted
	if (Role == 3) // Special — vibrant with accent
	{
		Body = FColor(
			FMath::Clamp(Body.R + 30, 0, 255),
			FMath::Clamp(Body.G + 20, 0, 255),
			FMath::Clamp(Body.B + 10, 0, 255), 255);
	}

	// Draw the animal body for this empire
	switch (Empire)
	{
	case 0: DrawPandaBody(P, Size, Frame, Body, Accent); break;
	case 1: DrawBunnyBody(P, Size, Frame, Body, Accent); break;
	case 2: DrawMonkeyBody(P, Size, Frame, Body, Accent); break;
	case 3: DrawGorillaBody(P, Size, Frame, Body, Accent); break;
	case 4: DrawCatBody(P, Size, Frame, Body, Accent); break;
	}

	// Add weapon overlays based on role
	if (Role == 1) // Warrior — weapon
	{
		FillEllipse(P, Size, Size * 0.75f, Size * 0.50f, Size * 0.03f, Size * 0.15f, FColor(120, 90, 50, 255)); // club/sword
	}
	else if (Role == 2) // Archer — bow
	{
		for (int i = 0; i < 15; ++i)
		{
			float T = (float)i / 15.0f;
			float BX = Size * 0.78f + FMath::Sin(T * 3.14f) * Size * 0.05f;
			float BY = Size * 0.35f + T * Size * 0.30f;
			SetPixel(P, Size, BX, BY, FColor(100, 70, 40, 255));
		}
	}
	else if (Role == 3) // Special — aura/crown
	{
		for (int i = 0; i < 12; ++i)
		{
			float A = i * (6.283f / 12.0f) + Frame * 0.1f;
			float AX = Size * 0.5f + FMath::Cos(A) * Size * 0.20f;
			float AY = Size * 0.20f + FMath::Sin(A) * Size * 0.05f;
			FillCircle(P, Size, AX, AY, Size * 0.012f, FColor(255, 220, 100, 200));
		}
	}

	// Post-processing: fur texture, shading, highlights, outline, shadow
	AddFurTexture(P, Size, Body, 0.25f);
	AddShading(P, Size, 0.7f);
	AddHighlights(P, Size, 0.12f);
	DrawOutline(P, Size, FColor(20, 15, 10, 200));
	AddDropShadow(P, Size);

	// Flip horizontally for left-facing directions (Dir 5,6,7 = SW, W, NW)
	if (Dir >= 5 && Dir <= 7)
	{
		uint8* Flipped = new uint8[Size * Size * 4];
		for (int Y = 0; Y < Size; ++Y)
			for (int X = 0; X < Size; ++X)
			{
				int SrcIdx = (Y * Size + X) * 4;
				int DstIdx = (Y * Size + (Size - 1 - X)) * 4;
				Flipped[DstIdx] = P[SrcIdx];
				Flipped[DstIdx + 1] = P[SrcIdx + 1];
				Flipped[DstIdx + 2] = P[SrcIdx + 2];
				Flipped[DstIdx + 3] = P[SrcIdx + 3];
			}
		FMemory::Memcpy(P, Flipped, Size * Size * 4);
		delete[] Flipped;
	}

	FString Name = FString::Printf(TEXT("RT_UnitSprite_E%d_R%d_F%d_D%d"), Empire, Role, Frame, Dir);
	UTexture2D* Tex = CreateTextureFromPixels(P, Size, Name);
	delete[] P;
	return Tex;
}

UTexture2D* UAoASpriteGenerator::GenerateBuildingSprite(int32 Empire, int32 Role, int32 Size)
{
	uint8* P = new uint8[Size * Size * 4];
	FMemory::Memset(P, 0, Size * Size * 4);

	FColor Body = GetEmpireBodyColor(Empire);
	FColor Accent = GetEmpireAccentColor(Empire);

	// Building color palette — use empire colors
	FColor WallColor = FColor(
		FMath::Clamp(Body.R * 0.7f + 40, 0, 255),
		FMath::Clamp(Body.G * 0.7f + 35, 0, 255),
		FMath::Clamp(Body.B * 0.7f + 30, 0, 255), 255);
	FColor RoofColor = Accent;
	FColor DetailColor = FColor(Accent.R * 0.6f, Accent.G * 0.6f, Accent.B * 0.6f, 255);

	float CX = Size * 0.5f;
	float BaseY = Size * 0.75f;
	float WallW = Size * 0.35f;
	float WallH = Size * 0.22f;
	float RoofH = Size * 0.10f;

	switch (Role)
	{
	case 0: // Town Center — large
		WallW = Size * 0.32f;
		WallH = Size * 0.25f;
		RoofH = Size * 0.15f;
		break;
	case 1: // House — small
		WallW = Size * 0.18f;
		WallH = Size * 0.15f;
		RoofH = Size * 0.08f;
		break;
	case 2: // Barracks — medium
		WallW = Size * 0.25f;
		WallH = Size * 0.20f;
		RoofH = Size * 0.10f;
		break;
	case 3: // Tower — tall
		WallW = Size * 0.12f;
		WallH = Size * 0.35f;
		RoofH = Size * 0.12f;
		break;
	}

	// Walls
	FillEllipse(P, Size, CX, BaseY, WallW, WallH, WallColor);

	// Roof — triangular/conical
	for (int Y = 0; Y < RoofH; ++Y)
	{
		float T = (float)Y / RoofH;
		float W = WallW * (1.0f - T * 0.7f);
		float YPos = BaseY - WallH * 0.5f - Y;
		FillEllipse(P, Size, CX, YPos, W, 2.0f, RoofColor);
	}

	// Door
	FillEllipse(P, Size, CX, BaseY + WallH * 0.3f, WallW * 0.12f, WallH * 0.25f, FColor(40, 25, 15, 255));

	// Windows
	if (Role >= 2)
	{
		FillEllipse(P, Size, CX - WallW * 0.5f, BaseY - WallH * 0.1f, WallW * 0.08f, WallH * 0.08f, FColor(200, 180, 80, 255));
		FillEllipse(P, Size, CX + WallW * 0.5f, BaseY - WallH * 0.1f, WallW * 0.08f, WallH * 0.08f, FColor(200, 180, 80, 255));
	}

	// Banner/flag with empire color
	FillEllipse(P, Size, CX, BaseY - WallH - RoofH, WallW * 0.08f, 2.0f, FColor(80, 60, 40, 255));
	FColor FlagColor = FColor(
		GetEmpireBodyColor(Empire).R * 0.8f,
		GetEmpireBodyColor(Empire).G * 0.8f,
		GetEmpireBodyColor(Empire).B * 0.8f, 255);
	FillEllipse(P, Size, CX + WallW * 0.15f, BaseY - WallH - RoofH - Size * 0.03f, WallW * 0.12f, Size * 0.03f, FlagColor);

	// Post-processing
	AddShading(P, Size, 0.8f);
	AddHighlights(P, Size, 0.10f);
	DrawOutline(P, Size, FColor(15, 10, 5, 200));
	AddDropShadow(P, Size);

	FString Name = FString::Printf(TEXT("RT_BuildingSprite_E%d_R%d"), Empire, Role);
	UTexture2D* Tex = CreateTextureFromPixels(P, Size, Name);
	delete[] P;
	return Tex;
}

UTexture2D* UAoASpriteGenerator::GenerateTerrainTexture(int32 TileType, int32 Size)
{
	uint8* P = new uint8[Size * Size * 4];

	// Base colors for each terrain type
	FColor BaseColors[] = {
		FColor(60, 140, 56, 255),   // Grass
		FColor(30, 95, 200, 255),   // Water
		FColor(25, 90, 38, 255),    // Forest
		FColor(132, 178, 40, 255),  // Bamboo
		FColor(132, 128, 118, 255), // Rock
		FColor(127, 87, 50, 255),   // Dirt
		FColor(209, 189, 128, 255), // Sand
		FColor(140, 127, 56, 255),  // Flowers
	};
	FColor Base = BaseColors[FMath::Clamp(TileType, 0, 7)];

	FRandomStream RNG(42 + TileType);
	for (int Y = 0; Y < Size; ++Y)
		for (int X = 0; X < Size; ++X)
		{
			// Add per-pixel noise for texture detail
			float N = RNG.FRand();
			int32 Variation = (int32)(N * 30.0f) - 15;
			FColor C(
				FMath::Clamp(Base.R + Variation, 0, 255),
				FMath::Clamp(Base.G + Variation, 0, 255),
				FMath::Clamp(Base.B + Variation, 0, 255), 255);
			SetPixel(P, Size, X, Y, C);
		}

	// Add terrain-specific details
	if (TileType == 2) // Forest — tree dots
	{
		for (int i = 0; i < 8; ++i)
		{
			float TX = RNG.FRand() * Size;
			float TY = RNG.FRand() * Size;
			FillCircle(P, Size, TX, TY, Size * 0.06f, FColor(20, 70, 25, 255));
			FillCircle(P, Size, TX, TY - 2, Size * 0.04f, FColor(35, 100, 35, 255));
		}
	}
	else if (TileType == 3) // Bamboo — bamboo stalks
	{
		for (int i = 0; i < 6; ++i)
		{
			float TX = RNG.FRand() * Size;
			FillEllipse(P, Size, TX, Size * 0.5f, Size * 0.03f, Size * 0.15f, FColor(100, 160, 30, 255));
		}
	}
	else if (TileType == 4) // Rock — stone clusters
	{
		for (int i = 0; i < 5; ++i)
		{
			float RX = RNG.FRand() * Size;
			float RY = RNG.FRand() * Size;
			FillCircle(P, Size, RX, RY, Size * 0.05f, FColor(100, 96, 90, 255));
		}
	}
	else if (TileType == 7) // Flowers
	{
		for (int i = 0; i < 10; ++i)
		{
			float FX = RNG.FRand() * Size;
			float FY = RNG.FRand() * Size;
			FColor FlowerColors[] = {
				FColor(255, 100, 100, 255),
				FColor(255, 200, 80, 255),
				FColor(200, 150, 255, 255),
				FColor(255, 255, 150, 255),
			};
			FillCircle(P, Size, FX, FY, Size * 0.02f, FlowerColors[RNG.RandRange(0, 3)]);
		}
	}
	else if (TileType == 1) // Water — ripples
	{
		for (int i = 0; i < 5; ++i)
		{
			float WY = RNG.FRand() * Size;
			float WX = RNG.FRand() * Size;
			for (int X = 0; X < Size * 0.15f; ++X)
			{
				SetPixel(P, Size, WX + X, WY, FColor(60, 120, 220, 255));
				SetPixel(P, Size, WX + X, WY + 1, FColor(60, 120, 220, 200));
			}
		}
	}

	FString Name = FString::Printf(TEXT("RT_Terrain_T%d"), TileType);
	UTexture2D* Tex = CreateTextureFromPixels(P, Size, Name);
	delete[] P;
	return Tex;
}

UTexture2D* UAoASpriteGenerator::GenerateEmpirePortrait(int32 Empire, int32 Size)
{
	// Generate a larger, detailed portrait for UI use
	uint8* P = new uint8[Size * Size * 4];
	FMemory::Memset(P, 0, Size * Size * 4);

	FColor Body = GetEmpireBodyColor(Empire);
	FColor Accent = GetEmpireAccentColor(Empire);

	// Background gradient
	for (int Y = 0; Y < Size; ++Y)
		for (int X = 0; X < Size; ++X)
		{
			float T = (float)Y / Size;
			FColor BG(
				FMath::Lerp(30, 50, T),
				FMath::Lerp(30, 45, T),
				FMath::Lerp(40, 55, T), 255);
			SetPixel(P, Size, X, Y, BG);
		}

	// Draw animal body (using frame 0)
	switch (Empire)
	{
	case 0: DrawPandaBody(P, Size, 0, Body, Accent); break;
	case 1: DrawBunnyBody(P, Size, 0, Body, Accent); break;
	case 2: DrawMonkeyBody(P, Size, 0, Body, Accent); break;
	case 3: DrawGorillaBody(P, Size, 0, Body, Accent); break;
	case 4: DrawCatBody(P, Size, 0, Body, Accent); break;
	}

	AddFurTexture(P, Size, Body, 0.30f);
	AddShading(P, Size, 0.7f);
	AddHighlights(P, Size, 0.15f);
	DrawOutline(P, Size, FColor(10, 8, 5, 220));

	FString Name = FString::Printf(TEXT("RT_Portrait_E%d"), Empire);
	UTexture2D* Tex = CreateTextureFromPixels(P, Size, Name);
	delete[] P;
	return Tex;
}

UPaperFlipbook* UAoASpriteGenerator::GenerateUnitFlipbook(int32 Empire, int32 Role, int32 Dir, int32 NumFrames)
{
	// In a complete build, this generates multiple sprite frames and creates
	// a UPaperFlipbook asset. This requires editor-only functionality.
	// For runtime, the textures are used directly via the sprite component.
	return nullptr;
}

void UAoASpriteGenerator::GenerateAllAssets()
{
	// Generate all unit sprites: 5 empires × 4 roles × 8 frames × 8 directions = 1280 sprites
	// Generate all building sprites: 5 empires × 4 roles = 20 sprites
	// Generate all terrain textures: 8 types
	// Generate all empire portraits: 5

	for (int32 E = 0; E < 5; ++E)
	{
		GenerateEmpirePortrait(E, 256);
		for (int32 R = 0; R < 4; ++R)
		{
			for (int32 F = 0; F < 8; ++F)
			{
				for (int32 D = 0; D < 8; ++D)
				{
					GenerateUnitSprite(E, R, F, D, 128);
				}
			}
			GenerateBuildingSprite(E, R, 256);
		}
	}
	for (int32 T = 0; T < 8; ++T)
	{
		GenerateTerrainTexture(T, 128);
	}
}

bool UAoASpriteGenerator::SaveTextureAsPNG(UTexture2D* Texture, const FString& Path)
{
	if (!Texture) return false;
	// Use UE's image write system
	// This is a simplified version — full implementation uses FImageWriteTask
	return false;
}

