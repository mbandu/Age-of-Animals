#include "AoAPlayerController.h"
#include "AoAPlayerState.h"
#include "AoAUnit.h"
#include "Engine/Engine.h"

AAoAPlayerController::AAoAPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAoAPlayerController::OnLeftClick()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	if (Hit.GetActor()) { SelectedActors.Empty(); SelectedActors.Add(Hit.GetActor()); }
	else { SelectedActors.Empty(); }
}

void AAoAPlayerController::OnRightClick()
{
	if (SelectedActors.IsEmpty()) return;
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);
	if (Hit.bBlockingHit) { IssueMoveCommand(Hit.Location); }
}

void AAoAPlayerController::IssueMoveCommand(const FVector& Location)
{
	for (AActor* Actor : SelectedActors)
	{
		if (auto* Unit = Cast<AAoAUnit>(Actor)) { Unit->CommandMove(Location); }
	}
}
