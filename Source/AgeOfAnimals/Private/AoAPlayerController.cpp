#include "AoAPlayerController.h"
#include "AoAPlayerState.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Components/InputComponent.h"

AAoAPlayerController::AAoAPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAoAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("LeftClick", IE_Pressed, this, &AAoAPlayerController::OnLeftClick);
		InputComponent->BindAction("RightClick", IE_Pressed, this, &AAoAPlayerController::OnRightClick);
	}
}

void AAoAPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetPawn())
	{
		FVector2D MousePos;
		GetMousePosition(MousePos.X, MousePos.Y);
		int32 VPX, VPY;
		GetViewportSize(VPX, VPY);
		FVector MoveDir = FVector::ZeroVector;
		if (MousePos.X < 20) MoveDir.Y -= 1;
		if (MousePos.X > VPX - 20) MoveDir.Y += 1;
		if (MousePos.Y < 20) MoveDir.X += 1;
		if (MousePos.Y > VPY - 20) MoveDir.X -= 1;
		if (!MoveDir.IsNearlyZero())
		{
			FVector Loc = GetPawn()->GetActorLocation();
			Loc += MoveDir.GetSafeNormal() * 2000.0f * DeltaTime;
			GetPawn()->SetActorLocation(Loc);
		}
	}
}

void AAoAPlayerController::OnLeftClick()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	if (Hit.GetActor())
	{
		SelectedActors.Empty();
		SelectedActors.Add(Hit.GetActor());
	}
	else
	{
		SelectedActors.Empty();
	}
}

void AAoAPlayerController::OnRightClick()
{
	if (SelectedActors.IsEmpty()) return;
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);
	if (Hit.bBlockingHit)
	{
		IssueMoveCommand(Hit.Location);
	}
}

void AAoAPlayerController::IssueMoveCommand(const FVector& Location)
{
	for (AActor* Actor : SelectedActors)
	{
		if (auto* Unit = Cast<AAoAUnit>(Actor))
		{
			Unit->CommandMove(Location);
		}
	}
}
