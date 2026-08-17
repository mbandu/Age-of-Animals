#include "AoAPlayerState.h"
AAoAPlayerState::AAoAPlayerState() { EmpireIndex = 0; }
bool AAoAPlayerState::SpendResources(int32 F, int32 W, int32 S, int32 G) { if (!CanAfford(F,W,S,G)) return false; Food-=F; Wood-=W; Stone-=S; Gold-=G; return true; }
void AAoAPlayerState::AddResource(EResourceType T, int32 A) { if(T==EResourceType::Food) Food+=A; else if(T==EResourceType::Wood) Wood+=A; else if(T==EResourceType::Stone) Stone+=A; else Gold+=A; }
bool AAoAPlayerState::CanAfford(int32 F, int32 W, int32 S, int32 G) const { return Food>=F && Wood>=W && Stone>=S && Gold>=G; }
