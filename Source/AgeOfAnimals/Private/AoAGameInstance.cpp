#include "AoAGameInstance.h"
void UAoAGameInstance::LoadEmpireAssets() {}
UObject* UAoAGameInstance::GetEmpireData(int32 EmpireIndex) const { return nullptr; }
void UAoAGameInstance::AddLobbyPlayer(const FString& Name, int32 EmpireIdx, bool bAI) { FAoALobbyPlayer P; P.PlayerName = Name; P.EmpireIndex = EmpireIdx; P.bIsAI = bAI; LobbyPlayers.Add(P); }
void UAoAGameInstance::SetLocalEmpire(int32 EmpireIndex) { LocalEmpireIndex = EmpireIndex; }
bool UAoAGameInstance::AllPlayersReady() const { return false; }
