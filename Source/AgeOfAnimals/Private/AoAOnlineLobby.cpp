#include "AoAOnlineLobby.h"
void UAoAOnlineLobby::RequestRoomList() {}
void UAoAOnlineLobby::StartAutoRefresh(float IntervalSeconds) {}
TArray<FAoAOnlineRoom> UAoAOnlineLobby::GetCachedRooms() const { return CachedRooms; }
