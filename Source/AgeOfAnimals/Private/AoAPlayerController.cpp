#include "EngineUtils.h"
#include "AoAPlayerController.h"
#include "AoAPlayerState.h"
#include "AoAUnit.h"
#include "AoABuilding.h"
#include "AoAResourceNode.h"
#include "AoAGameMode.h"
#include "AoAGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

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

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Enhanced input bindings would go here with InputAction assets
	}
	else
	{
		// Fallback to legacy input
		InputComponent->BindAction("LeftClick", IE_Pressed, this, &AAoAPlayerController::OnLeftMousePressed);
		InputComponent->BindAction("LeftClick", IE_Released, this, &AAoAPlayerController::OnLeftMouseReleased);
		InputComponent->BindAction("RightClick", IE_Pressed, this, &AAoAPlayerController::OnRightMousePressed);
		InputComponent->BindAxis("Scroll", this, &AAoAPlayerController::OnScroll);
		InputComponent->BindAxis("PanForward", this, &AAoAPlayerController::OnPanForward);
		InputComponent->BindAxis("PanRight", this, &AAoAPlayerController::OnPanRight);
		InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AAoAPlayerController::OnZoomIn);
		InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AAoAPlayerController::OnZoomOut);
	}
}

void AAoAPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCamera(DeltaTime);
	EdgePan(DeltaTime);

	if (bIsDragging)
	{
		FVector2D MousePos;
		GetMousePosition(MousePos.X, MousePos.Y);
		DragEnd = MousePos;
	}
}

// ---- Camera ----
void AAoAPlayerController::UpdateCamera(float DeltaTime)
{
	if (!GetPawn()) return;

	FVector CamLoc = GetPawn()->GetActorLocation();
	FRotator CamRot = GetPawn()->GetActorRotation();

	// Pan from WASD input
	float ForwardVal = 0.0f, RightVal = 0.0f;
	// Read from input axis (set by OnPanForward/OnPanRight via cached values)
	// In a full implementation we'd use Enhanced Input; here we apply edge panning

	GetPawn()->SetActorLocation(CamLoc);
}

void AAoAPlayerController::EdgePan(float DeltaTime)
{
	if (!GetPawn()) return;

	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);
	int32 VPX, VPY;
	GetViewportSize(VPX, VPY);

	FVector MoveDir = FVector::ZeroVector;
	if (MousePos.X < EdgePanMargin) MoveDir.Y -= 1.0f;
	if (MousePos.X > VPX - EdgePanMargin) MoveDir.Y += 1.0f;
	if (MousePos.Y < EdgePanMargin) MoveDir.X += 1.0f;
	if (MousePos.Y > VPY - EdgePanMargin) MoveDir.X -= 1.0f;

	if (!MoveDir.IsNearlyZero())
	{
		FVector CamLoc = GetPawn()->GetActorLocation();
		CamLoc += MoveDir.GetSafeNormal() * EdgePanSpeed * DeltaTime;
		GetPawn()->SetActorLocation(CamLoc);
	}
}

void AAoAPlayerController::OnScroll(float Delta)
{
	CurrentZoom = FMath::Clamp(CurrentZoom - Delta * ZoomSpeed, MinZoom, MaxZoom);
}

void AAoAPlayerController::OnPanForward(float Val) {}
void AAoAPlayerController::OnPanRight(float Val) {}
void AAoAPlayerController::OnZoomIn() { CurrentZoom = FMath::Clamp(CurrentZoom - ZoomSpeed, MinZoom, MaxZoom); }
void AAoAPlayerController::OnZoomOut() { CurrentZoom = FMath::Clamp(CurrentZoom + ZoomSpeed, MinZoom, MaxZoom); }

// ---- Selection ----
void AAoAPlayerController::StartDragSelection(FVector2D ScreenPos)
{
	bIsDragging = true;
	DragStart = ScreenPos;
	DragEnd = ScreenPos;
}

void AAoAPlayerController::UpdateDragSelection(FVector2D ScreenPos)
{
	if (bIsDragging) DragEnd = ScreenPos;
}

void AAoAPlayerController::FinishDragSelection()
{
	bIsDragging = false;
	ClearSelection();

	if (DragStart.Equals(DragEnd, 5.0f))
	{
		// Click-select single actor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
		if (Hit.GetActor())
		{
			auto* Unit = Cast<AAoAUnit>(Hit.GetActor());
			auto* Bld = Cast<AAoABuilding>(Hit.GetActor());
			if (Unit && OwnsUnit(Unit))
				SelectedActors.Add(Unit);
			else if (Bld && OwnsBuilding(Bld))
			{
				SelectedBuilding = Bld;
				SelectedActors.Add(Bld);
			}
		}
		return;
	}

	// Box-select all owned units in the drag rectangle
	FVector2D Min(FMath::Min(DragStart.X, DragEnd.X), FMath::Min(DragStart.Y, DragEnd.Y));
	FVector2D Max(FMath::Max(DragStart.X, DragEnd.X), FMath::Max(DragStart.Y, DragEnd.Y));

	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		AAoAUnit* Unit = *It;
		if (!OwnsUnit(Unit)) continue;

		FVector2D ScreenPos;
		ProjectWorldLocationToScreen(Unit->GetActorLocation(), ScreenPos);
		if (ScreenPos.X >= Min.X && ScreenPos.X <= Max.X &&
			ScreenPos.Y >= Min.Y && ScreenPos.Y <= Max.Y)
		{
			SelectedActors.Add(Unit);
		}
	}
}

bool AAoAPlayerController::OwnsUnit(AAoAUnit* Unit) const
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	return PS && Unit && Unit->GetOwnerPlayerId() == PS->GetPlayerId();
}

bool AAoAPlayerController::OwnsBuilding(AAoABuilding* Bld) const
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	return PS && Bld && Bld->GetOwnerPlayerId() == PS->GetPlayerId();
}

void AAoAPlayerController::ClearSelection()
{
	SelectedActors.Empty();
	SelectedBuilding = nullptr;
}

void AAoAPlayerController::SelectAllMilitary()
{
	ClearSelection();
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return;
	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() == PS->GetPlayerId() && !It->IsVillager())
			SelectedActors.Add(*It);
	}
}

void AAoAPlayerController::SelectAllVillagers()
{
	ClearSelection();
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return;
	for (TActorIterator<AAoAUnit> It(GetWorld()); It; ++It)
	{
		if (It->GetOwnerPlayerId() == PS->GetPlayerId() && It->IsVillager())
			SelectedActors.Add(*It);
	}
}

// ---- Mouse handlers ----
void AAoAPlayerController::OnLeftMousePressed()
{
	if (bIsPlacingBuilding && PendingBuildingClass)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);
		if (Hit.bBlockingHit)
		{
			IssueBuildCommand(PendingBuildingClass, Hit.Location);
			if (!bPlaceMultiple)
			{
				bIsPlacingBuilding = false;
				PendingBuildingClass = nullptr;
			}
		}
		return;
	}

	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);
	StartDragSelection(MousePos);
}

void AAoAPlayerController::OnLeftMouseReleased()
{
	if (bIsDragging)
		FinishDragSelection();
}

void AAoAPlayerController::OnRightMousePressed()
{
	if (SelectedActors.IsEmpty()) return;

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);
	AActor* HitActor = Hit.GetActor();

	ECommandType Cmd = DetermineRightClickAction(Hit.Location, HitActor);

	switch (Cmd)
	{
	case ECommandType::Move:
		IssueMoveCommand(Hit.Location);
		break;
	case ECommandType::AttackTarget:
		IssueAttackTargetCommand(HitActor);
		break;
	case ECommandType::Gather:
		if (auto* Res = Cast<AAoAResourceNode>(HitActor))
			IssueGatherCommand(Res);
		break;
	case ECommandType::Stop:
		IssueStopCommand();
		break;
	default:
		IssueMoveCommand(Hit.Location);
		break;
	}

	// If a building is selected, set rally point
	if (SelectedBuilding)
	{
		IssueRallyPointCommand(Cast<AAoABuilding>(SelectedBuilding.Get()), Hit.Location);
	}
}

ECommandType AAoAPlayerController::DetermineRightClickAction(const FVector& WorldLoc, AActor* HitActor)
{
	if (HitActor)
	{
		auto* EnemyUnit = Cast<AAoAUnit>(HitActor);
		auto* EnemyBld = Cast<AAoABuilding>(HitActor);
		auto* Resource = Cast<AAoAResourceNode>(HitActor);

		auto* PS = Cast<AAoAPlayerState>(PlayerState);
		if (PS)
		{
			if (EnemyUnit && EnemyUnit->GetOwnerPlayerId() != PS->GetPlayerId())
				return ECommandType::AttackTarget;
			if (EnemyBld && EnemyBld->GetOwnerPlayerId() != PS->GetPlayerId())
				return ECommandType::AttackTarget;
		}
		if (Resource)
			return ECommandType::Gather;
	}
	return ECommandType::Move;
}

// ---- Command issuers (client-side, call server RPCs) ----
void AAoAPlayerController::IssueMoveCommand(const FVector& Location)
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty())
		Server_Move(IDs, Location);
}

void AAoAPlayerController::IssueAttackTargetCommand(AActor* Target)
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty() && Target)
		Server_AttackTarget(IDs, Target);
}

void AAoAPlayerController::IssueAttackGroundCommand(const FVector& Location)
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty())
		Server_AttackGround(IDs, Location);
}

void AAoAPlayerController::IssueGatherCommand(AAoAResourceNode* Resource)
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty() && Resource)
		Server_Gather(IDs, Resource);
}

void AAoAPlayerController::IssueGatherNearestCommand(EResourceType ResType)
{
	// Find nearest resource of this type to the first selected unit
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (IDs.IsEmpty()) return;

	// Server-side search would be ideal; for simplicity, client finds and sends gather command
	// In production this would be a server RPC with the resource type
}

void AAoAPlayerController::IssueStopCommand()
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty())
		Server_Stop(IDs);
}

void AAoAPlayerController::IssueBuildCommand(TSubclassOf<AAoABuilding> BuildingClass, const FVector& Location)
{
	TArray<uint32> IDs = GetSelectedUnitIDs();
	if (!IDs.IsEmpty())
		Server_Build(IDs, BuildingClass, Location);
}

void AAoAPlayerController::IssueTrainCommand(AAoABuilding* Building, TSubclassOf<AAoAUnit> UnitClass)
{
	if (Building && UnitClass)
		Server_Train(Building, UnitClass);
}

void AAoAPlayerController::IssueRallyPointCommand(AAoABuilding* Building, const FVector& Location)
{
	if (Building)
		Server_SetRally(Building, Location);
}

void AAoAPlayerController::IssueAgeUpCommand()
{
	Server_AgeUp();
}

void AAoAPlayerController::RequestEmpireChange(int32 EmpireIndex)
{
	Server_SetEmpire(EmpireIndex);
}

TArray<uint32> AAoAPlayerController::GetSelectedUnitIDs() const
{
	TArray<uint32> IDs;
	for (auto* Actor : SelectedActors)
	{
		if (auto* Unit = Cast<AAoAUnit>(Actor))
			IDs.Add(Unit->GetNetworkID());
	}
	return IDs;
}

// ---- Server RPCs ----
void AAoAPlayerController::Server_Move_Implementation(const TArray<uint32>& UnitIDs, FVector Location)
{
	for (uint32 ID : UnitIDs)
	{
		if (auto* U = AAoAUnit::FindByID(GetWorld(), ID))
			U->CommandMove(Location);
	}
}
bool AAoAPlayerController::Server_Move_Validate(const TArray<uint32>&, FVector) { return true; }

void AAoAPlayerController::Server_AttackTarget_Implementation(const TArray<uint32>& UnitIDs, AActor* Target)
{
	for (uint32 ID : UnitIDs)
	{
		if (auto* U = AAoAUnit::FindByID(GetWorld(), ID))
			U->CommandAttackTarget(Target);
	}
}
bool AAoAPlayerController::Server_AttackTarget_Validate(const TArray<uint32>&, AActor*) { return true; }

void AAoAPlayerController::Server_AttackGround_Implementation(const TArray<uint32>& UnitIDs, FVector Location)
{
	for (uint32 ID : UnitIDs)
	{
		if (auto* U = AAoAUnit::FindByID(GetWorld(), ID))
			U->CommandAttackGround(Location);
	}
}
bool AAoAPlayerController::Server_AttackGround_Validate(const TArray<uint32>&, FVector) { return true; }

void AAoAPlayerController::Server_Gather_Implementation(const TArray<uint32>& UnitIDs, AActor* Resource)
{
	auto* ResNode = Cast<AAoAResourceNode>(Resource);
	for (uint32 ID : UnitIDs)
	{
		if (auto* U = AAoAUnit::FindByID(GetWorld(), ID))
			U->CommandGather(ResNode);
	}
}
bool AAoAPlayerController::Server_Gather_Validate(const TArray<uint32>&, AActor*) { return true; }

void AAoAPlayerController::Server_Stop_Implementation(const TArray<uint32>& UnitIDs)
{
	for (uint32 ID : UnitIDs)
	{
		if (auto* U = AAoAUnit::FindByID(GetWorld(), ID))
			U->CommandStop();
	}
}
bool AAoAPlayerController::Server_Stop_Validate(const TArray<uint32>&) { return true; }

void AAoAPlayerController::Server_Build_Implementation(const TArray<uint32>& UnitIDs, TSubclassOf<AAoABuilding> BuildingClass, FVector Location)
{
	if (UnitIDs.IsEmpty()) return;
	auto* Builder = AAoAUnit::FindByID(GetWorld(), UnitIDs[0]);
	if (Builder)
		Builder->CommandBuild(BuildingClass, Location);
}
bool AAoAPlayerController::Server_Build_Validate(const TArray<uint32>&, TSubclassOf<AAoABuilding>, FVector) { return true; }

void AAoAPlayerController::Server_Train_Implementation(AAoABuilding* Building, TSubclassOf<AAoAUnit> UnitClass)
{
	if (Building)
		Building->TrainUnit(UnitClass);
}
bool AAoAPlayerController::Server_Train_Validate(AAoABuilding*, TSubclassOf<AAoAUnit>) { return true; }

void AAoAPlayerController::Server_SetRally_Implementation(AAoABuilding* Building, FVector Location)
{
	if (Building)
		Building->SetRallyPoint(Location);
}
bool AAoAPlayerController::Server_SetRally_Validate(AAoABuilding*, FVector) { return true; }

void AAoAPlayerController::Server_AgeUp_Implementation()
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (!PS) return;
	// Check cost and advance age
	// AgeUpCost depends on current age
	// Implementation checks resources and advances
}
bool AAoAPlayerController::Server_AgeUp_Validate() { return true; }

void AAoAPlayerController::Server_SetEmpire_Implementation(int32 EmpireIndex)
{
	auto* PS = Cast<AAoAPlayerState>(PlayerState);
	if (PS) PS->EmpireIndex = EmpireIndex;
}
bool AAoAPlayerController::Server_SetEmpire_Validate(int32) { return true; }

