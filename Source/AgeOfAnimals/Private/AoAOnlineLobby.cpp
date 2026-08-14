#include "AoAOnlineLobby.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonUtilities.h"
#include "JsonObjectConverter.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

void UAoAOnlineLobby::SendRequest(const FString& Endpoint, const FString& Body, TFunction<void(bool, const FString&)> Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(ServerURL + Endpoint);
	Request->SetVerb(Body.IsEmpty() ? TEXT("GET") : TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	if (!Body.IsEmpty())
		Request->SetContentAsString(Body);

	Request->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
		{
			if (bSuccess && Response.IsValid())
			{
				Callback(true, Response->GetContentAsString());
			}
			else
			{
				Callback(false, TEXT("Request failed"));
			}
		});

	Request->ProcessRequest();
}

void UAoAOnlineLobby::RequestRoomList()
{
	if (bIsRefreshing) return;
	bIsRefreshing = true;

	SendRequest(TEXT("/api/rooms"), TEXT(""), [this](bool bSuccess, const FString& Response)
	{
		bIsRefreshing = false;
		if (!bSuccess)
		{
			OnLobbyError.Broadcast(TEXT("Failed to fetch room list"));
			return;
		}

		// Parse JSON array of rooms
		TArray<FAoAOnlineRoom> ParsedRooms;
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);

		if (FJsonSerializer::Deserialize(Reader, RootObj))
		{
			const TArray<TSharedPtr<FJsonValue>>* RoomsArray;
			if (RootObj->TryGetArrayField(TEXT("rooms"), RoomsArray))
			{
				for (const auto& RoomVal : *RoomsArray)
				{
					TSharedPtr<FJsonObject> RoomObj = RoomVal->AsObject();
					if (!RoomObj) continue;
					FAoAOnlineRoom Room;
					Room.RoomID = RoomObj->GetStringField(TEXT("id"));
					Room.HostName = RoomObj->GetStringField(TEXT("hostName"));
					Room.HostEmpireIndex = RoomObj->GetIntegerField(TEXT("hostEmpire"));
					Room.NumPlayers = RoomObj->GetIntegerField(TEXT("numPlayers"));
					Room.MaxPlayers = RoomObj->GetIntegerField(TEXT("maxPlayers"));
					Room.HostIP = RoomObj->GetStringField(TEXT("hostIP"));
					Room.HostPort = RoomObj->GetIntegerField(TEXT("hostPort"));
					Room.bIsPrivate = RoomObj->GetBoolField(TEXT("isPrivate"));
					Room.MapName = RoomObj->GetStringField(TEXT("mapName"));
					ParsedRooms.Add(Room);
				}
			}
		}

		CachedRooms = ParsedRooms;
		OnRoomsUpdated.Broadcast(CachedRooms);
	});
}

void UAoAOnlineLobby::CreateRoom(const FString& RoomName, int32 MaxPlayers, const FString& MapName, bool bPrivate)
{
	TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
	Body->SetStringField(TEXT("roomName"), RoomName);
	Body->SetStringField(TEXT("playerName"), PlayerName);
	Body->SetNumberField(TEXT("empireIndex"), SelectedEmpire);
	Body->SetNumberField(TEXT("maxPlayers"), MaxPlayers);
	Body->SetStringField(TEXT("mapName"), MapName);
	Body->SetBoolField(TEXT("isPrivate"), bPrivate);

	FString BodyStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

	SendRequest(TEXT("/api/rooms/create"), BodyStr, [this](bool bSuccess, const FString& Response)
	{
		if (!bSuccess)
		{
			OnLobbyError.Broadcast(Response);
			return;
		}

		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		if (FJsonSerializer::Deserialize(Reader, RootObj))
		{
			FAoAOnlineRoom Room;
			Room.RoomID = RootObj->GetStringField(TEXT("id"));
			Room.HostName = RootObj->GetStringField(TEXT("hostName"));
			Room.HostIP = RootObj->GetStringField(TEXT("hostIP"));
			Room.HostPort = RootObj->GetIntegerField(TEXT("hostPort"));
			CurrentRoomID = Room.RoomID;
			OnRoomCreated.Broadcast(Room);
		}
	});
}

void UAoAOnlineLobby::JoinRoom(const FString& RoomID)
{
	TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
	Body->SetStringField(TEXT("playerName"), PlayerName);
	Body->SetNumberField(TEXT("empireIndex"), SelectedEmpire);

	FString BodyStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

	SendRequest(FString::Printf(TEXT("/api/rooms/%s/join"), *RoomID), BodyStr, [this, RoomID](bool bSuccess, const FString& Response)
	{
		if (!bSuccess)
		{
			OnJoinResult.Broadcast(false);
			OnLobbyError.Broadcast(Response);
			return;
		}

		// Parse the host IP:port from the response
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		FString HostIP;
		int32 HostPort = 7777;
		if (FJsonSerializer::Deserialize(Reader, RootObj))
		{
			HostIP = RootObj->GetStringField(TEXT("hostIP"));
			HostPort = RootObj->GetIntegerField(TEXT("hostPort"));
		}

		CurrentRoomID = RoomID;
		OnJoinResult.Broadcast(true);

		// Connect to the host's game server using UE's OpenLevel
		// The actual connection is handled by the GameInstance:
		// GetGameInstance()->GetEngine()->AddNetworkedHost(HostIP, HostPort);
	});
}

void UAoAOnlineLobby::LeaveRoom()
{
	if (CurrentRoomID.IsEmpty()) return;

	TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
	Body->SetStringField(TEXT("playerName"), PlayerName);

	FString BodyStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);

	SendRequest(FString::Printf(TEXT("/api/rooms/%s/leave"), *CurrentRoomID), BodyStr,
		[this](bool, const FString&) { CurrentRoomID.Empty(); });
}

void UAoAOnlineLobby::StartAutoRefresh(float IntervalSeconds)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RefreshTimer, this, &UAoAOnlineLobby::RequestRoomList, IntervalSeconds, true);
	}
}

void UAoAOnlineLobby::StopAutoRefresh()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
	}
}
