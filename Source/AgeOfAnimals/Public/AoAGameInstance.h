#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AoAGameInstance.generated.h"

class UAoAEmpireData;

USTRUCT(BlueprintType)
struct FAoALobbyPlayer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 EmpireIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	bool bIsAI = false;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	bool bIsReady = false;
};

/**
 * Holds session-level state: the selected empire, lobby player list, and
 * online connection settings. Persists across level transitions.
 */
UCLASS(BlueprintType)
class UAoAGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Game")
	TArray<FAoALobbyPlayer> LobbyPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	int32 LocalEmpireIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	FString LocalPlayerName = TEXT("Player");

	UPROPERTY(BlueprintReadWrite, Category = "Network")
	FString ServerAddress = TEXT("127.0.0.1");

	UPROPERTY(BlueprintReadWrite, Category = "Network")
	int32 ServerPort = 7777;

	UPROPERTY(BlueprintReadWrite, Category = "Online")
	FString LobbyServerURL = TEXT("http://localhost:3000");

	UPROPERTY(BlueprintReadWrite, Category = "Online")
	FString MatchID;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	bool bIsOnlineMode = false;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	bool bIsHost = false;

	/** Empire data assets loaded from Content/ — indexed by EAnimalEmpire */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UObject*> EmpireAssets;

	virtual void Init() override;

	/** Load all empire data assets from the content registry */
	UFUNCTION(BlueprintCallable, Category = "Data")
	void LoadEmpireAssets();

	UFUNCTION(BlueprintCallable, Category = "Data")
	UAoAEmpireData* GetEmpireData(int32 EmpireIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void AddLobbyPlayer(const FString& Name, int32 EmpireIdx, bool bAI);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RemoveLobbyPlayer(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetLocalEmpire(int32 EmpireIndex);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetPlayerReady(int32 Index, bool bReady);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool AllPlayersReady() const;
};
