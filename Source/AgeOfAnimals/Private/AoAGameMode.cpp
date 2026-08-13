#include "AoAGameMode.h"
#include "AoAGameState.h"
#include "AoAPlayerState.h"
#include "AoAPlayerController.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAGameInstance.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

AAoAGameMode::AAoAGameMode()
{
	// Use our custom classes
	static ConstructorHelpers::FClassFinder<APlayerController> PCFinder(
		TEXT("/Game/Blueprints/BP_AoAPlayerController"));
	if (PCFinder.Succeeded())
		PlayerControllerClass = PCFinder.Class;
	else
		PlayerControllerClass = AAoAPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<AGameStateBase> GSFinder(
		TEXT("/Game/Blueprints/BP_AoAGameState"));
	if (GSFinder.Succeeded())
		GameStateClass = GSFinder.Class;
	else
		GameStateClass = AAoAGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<APlayerState> PSFinder(
		TEXT("/Game/Blueprints/BP_AoAPlayerState"));
	if (PSFinder.Succeeded())
		PlayerStateClass = PSFinder.Class;
	else
		PlayerStateClass = AAoAPlayerState::StaticClass();

	bUseSeamlessTravel = true;
	MinNetUpdateFrequency = 20.0f;
}

void AAoAGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (RandomSeed == 0)
		RandomSeed = FMath::Rand();

	auto* GI = Cast<UAoAGameInstance>(GetGameInstance());
	if (GI)
	{
		// Determine match type from options
		FString MatchTypeStr = UGameplayStatics::ParseOption(Options, TEXT("MatchType"));
		if (MatchTypeStr == TEXT("LAN"))
			MatchType = EMatchType::LANHost;
		else if (MatchTypeStr == TEXT("Online"))
			MatchType = EMatchType::OnlineMatch;
	}

	if (auto* GS = Cast<AAoAGameState>(GameState))
	{
		GS->MatchSeed = FString::Printf(TEXT("%u"), RandomSeed);
		GS->Phase = EGamePhase::Preparing;
	}
}

void AAoAGameMode::SpawnStartingBase(AAoAPlayerController* PC, const FVector& Location)
{
	if (!PC) return;
	auto* PS = Cast<AAoAPlayerState>(PC->PlayerState);
	if (!PS) return;

	// Spawn town center
	if (TownCenterClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PC;
		auto* TC = GetWorld()->SpawnActor<AAoABuilding>(TownCenterClass, Location, FRotator::ZeroRotator, SpawnParams);
		if (TC)
		{
			TC->SetOwnerPlayer(PS->GetPlayerId());
			TC->SetEmpireIndex(PS->EmpireIndex);
			TC->FinishConstruction();
		}
	}

	// Spawn starting villagers around the TC
	if (VillagerClass)
	{
		for (int32 i = 0; i < StartVillagers; ++i)
		{
			float Angle = i * (2.0f * PI / StartVillagers);
			FVector Offset(FMath::Cos(Angle) * 200.0f, FMath::Sin(Angle) * 200.0f, 0.0f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			auto* Villager = GetWorld()->SpawnActor<AAoAUnit>(VillagerClass, Location + Offset, FRotator::ZeroRotator, SpawnParams);
			if (Villager)
			{
				Villager->SetOwnerPlayer(PS->GetPlayerId());
				Villager->SetEmpireIndex(PS->EmpireIndex);
			}
		}
	}
}

FVector AAoAGameMode::FindSpawnLocation() const
{
	// Spread players across the map
	float MapExtent = MapSize.X * 100.0f; // 1 tile = 100 cm
	TArray<FVector> Candidates;
	Candidates.Add(FVector(MapExtent * 0.2f, MapExtent * 0.2f, 0.0f));
	Candidates.Add(FVector(MapExtent * 0.8f, MapExtent * 0.8f, 0.0f));
	Candidates.Add(FVector(MapExtent * 0.2f, MapExtent * 0.8f, 0.0f));
	Candidates.Add(FVector(MapExtent * 0.8f, MapExtent * 0.2f, 0.0f));
	Candidates.Add(FVector(MapExtent * 0.5f, MapExtent * 0.5f, 0.0f));

	int32 Index = UsedSpawnLocations.Num();
	if (Candidates.IsValidIndex(Index))
		return Candidates[Index];
	return FVector(MapExtent * 0.5f, MapExtent * 0.5f, 0.0f);
}

void AAoAGameMode::GiveStartingResources(AAoAPlayerState* PS)
{
	if (!PS) return;
	PS->Food = StartFood;
	PS->Wood = StartWood;
	PS->Stone = StartStone;
}

void AAoAGameMode::NotifyPlayerEliminated(int32 PlayerId)
{
	if (auto* GS = Cast<AAoAGameState>(GameState))
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			auto* PS = Cast<AAoAPlayerState>(It->Get()->PlayerState);
			if (PS && PS->GetPlayerId() == PlayerId)
			{
				PS->bIsAlive = false;
			}
		}
		CheckWinCondition();
	}
}

void AAoAGameMode::CheckWinCondition()
{
	if (auto* GS = Cast<AAoAGameState>(GameState))
	{
		TArray<int32> AlivePlayers;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			auto* PS = Cast<AAoAPlayerState>(It->Get()->PlayerState);
			if (PS && PS->bIsAlive)
				AlivePlayers.Add(PS->GetPlayerId());
		}

		if (AlivePlayers.Num() == 1)
		{
			GS->WinnerPlayerId = AlivePlayers[0];
			GS->Phase = EGamePhase::Ended;
		}
		else if (AlivePlayers.Num() == 0)
		{
			GS->WinnerPlayerId = -1; // draw
			GS->Phase = EGamePhase::Ended;
		}
	}
}
