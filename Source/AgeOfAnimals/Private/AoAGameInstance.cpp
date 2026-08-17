#include "AoAGameInstance.h"
#include "AoAEmpireData.h"
#include "Engine/AssetManager.h"

void UAoAGameInstance::Init()
{
	Super::Init();
	LoadEmpireAssets();
}

void UAoAGameInstance::LoadEmpireAssets()
{
	EmpireAssets.SetNum(5);
	// In a packaged build these are loaded via Asset Manager.
	// In editor, they are found under /Game/Data/Empires/
	const FString Paths[] = {
		TEXT("/Game/Data/Empires/DA_PandaEmpire.DA_PandaEmpire"),
		TEXT("/Game/Data/Empires/DA_BunnyEmpire.DA_BunnyEmpire"),
		TEXT("/Game/Data/Empires/DA_MonkeyEmpire.DA_MonkeyEmpire"),
		TEXT("/Game/Data/Empires/DA_GorillaEmpire.DA_GorillaEmpire"),
		TEXT("/Game/Data/Empires/DA_CatEmpire.DA_CatEmpire"),
	};
	for (int32 i = 0; i < 5; ++i)
	{
		EmpireAssets[i] = LoadObject<UAoAEmpireData>(nullptr, *Paths[i]);
	}
}

UAoAEmpireData* UAoAGameInstance::GetEmpireData(int32 EmpireIndex) const
{
	if (EmpireAssets.IsValidIndex(EmpireIndex))
		return Cast<UAoAEmpireData>(EmpireAssets[EmpireIndex]);
	return nullptr;
}

void UAoAGameInstance::AddLobbyPlayer(const FString& Name, int32 EmpireIdx, bool bAI)
{
	FAoALobbyPlayer P;
	P.PlayerName = Name;
	P.EmpireIndex = EmpireIdx;
	P.bIsAI = bAI;
	LobbyPlayers.Add(P);
}

void UAoAGameInstance::RemoveLobbyPlayer(int32 Index)
{
	LobbyPlayers.RemoveAt(Index);
}

void UAoAGameInstance::SetLocalEmpire(int32 EmpireIndex)
{
	LocalEmpireIndex = EmpireIndex;
}

void UAoAGameInstance::SetPlayerReady(int32 Index, bool bReady)
{
	if (LobbyPlayers.IsValidIndex(Index))
		LobbyPlayers[Index].bIsReady = bReady;
}

bool UAoAGameInstance::AllPlayersReady() const
{
	if (LobbyPlayers.IsEmpty()) return false;
	for (const auto& P : LobbyPlayers)
		if (!P.bIsReady) return false;
	return true;
}
