#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "TimerManager.h"
#include "AoAOnlineLobby.generated.h"

/** A single room/match in the online lobby */
USTRUCT(BlueprintType)
struct FAoAOnlineRoom
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString RoomID;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString HostName;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 HostEmpireIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 NumPlayers = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 MaxPlayers = 5;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString HostIP;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 HostPort = 7777;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	bool bIsPrivate = false;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString MapName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomsUpdated, const TArray<FAoAOnlineRoom>&, Rooms);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCreated, const FAoAOnlineRoom&, Room);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinResult, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyError, const FString&, ErrorMessage);

/**
 * Online lobby client — communicates with the matchmaking server (Node.js)
 * to create/join rooms, list available games, and connect players for online play.
 * 
 * The matchmaking server runs separately (see Online/lobby_server.js) and
 * handles room management. Once matched, players connect directly via UE's
 * built-in network replication (listen server or dedicated server).
 */
UCLASS(BlueprintType)
class UAoAOnlineLobby : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString ServerURL = TEXT("http://localhost:3000");

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 SelectedEmpire = 0;

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnRoomsUpdated OnRoomsUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnRoomCreated OnRoomCreated;

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnJoinResult OnJoinResult;

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyError OnLobbyError;

	/** Get the list of available rooms from the server */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestRoomList();

	/** Create a new room (you become the host) */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void CreateRoom(const FString& RoomName, int32 MaxPlayers, const FString& MapName, bool bPrivate);

	/** Join an existing room by ID */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void JoinRoom(const FString& RoomID);

	/** Leave the current room */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void LeaveRoom();

	/** Refresh room list on a timer */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void StartAutoRefresh(float IntervalSeconds);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void StopAutoRefresh();

	/** Get the cached room list */
	UFUNCTION(BlueprintPure, Category = "Lobby")
	TArray<FAoAOnlineRoom> GetCachedRooms() const { return CachedRooms; }

protected:
	TArray<FAoAOnlineRoom> CachedRooms;
	FString CurrentRoomID;
	FTimerHandle RefreshTimer;
	bool bIsRefreshing = false;

	void SendRequest(const FString& Endpoint, const FString& Body, TFunction<void(bool, const FString&)> Callback);
};
