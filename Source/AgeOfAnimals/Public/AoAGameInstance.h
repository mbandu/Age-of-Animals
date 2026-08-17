#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AoAEmpireData.h"
struct FAoALobbyPlayer { FString PlayerName; int32 EmpireIndex = 0; bool bIsAI = false; bool bIsReady = false; };
class UAoAGameInstance : public UGameInstance
{
public:
	TArray<FAoALobbyPlayer> LobbyPlayers;
	int32 LocalEmpireIndex = 0;
	FString LocalPlayerName = TEXT("Player");
	TArray<UObject*> EmpireAssets;
	void LoadEmpireAssets();
	UObject* GetEmpireData(int32 EmpireIndex) const;
	void AddLobbyPlayer(const FString& Name, int32 EmpireIdx, bool bAI);
	void SetLocalEmpire(int32 EmpireIndex);
	bool AllPlayersReady() const;
};
