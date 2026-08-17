#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "TimerManager.h"
struct FAoAOnlineRoom { FString RoomID; FString HostName; int32 NumPlayers = 1; int32 MaxPlayers = 5; FString HostIP; int32 HostPort = 7777; bool bIsPrivate = false; FString MapName; };
class UAoAOnlineLobby : public UObject
{
public:
	FString ServerURL = TEXT("http://localhost:3000");
	FString PlayerName;
	int32 SelectedEmpire = 0;
	void RequestRoomList();
	void StartAutoRefresh(float IntervalSeconds);
	TArray<FAoAOnlineRoom> GetCachedRooms() const;
protected:
	TArray<FAoAOnlineRoom> CachedRooms;
	FTimerHandle RefreshTimer;
};
