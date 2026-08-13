#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AoAEmpireData.h"
#include "AoAPlayerController.generated.h"

class AAoAUnit;
class AAoABuilding;
class UAoAUnitData;
class UAoABuildingData;
class AAoAResourceNode;

UENUM(BlueprintType)
enum class ECommandType : uint8
{
	None,
	Move,
	AttackTarget,
	AttackGround,
	Gather,
	GatherNearest,
	Build,
	Stop,
	SetRally,
};

/**
 * RTS player controller — handles drag-selection, right-click commands,
 * camera panning/zooming, building placement, and training requests.
 * Commands are sent to the server via RPCs for authoritative execution.
 */
UCLASS(BlueprintType)
class AAoAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAoAPlayerController();

	// ---- Selection ----
	UPROPERTY(BlueprintReadWrite, Category = "Selection")
	TArray<TObjectPtr<AActor>> SelectedActors;

	UPROPERTY(BlueprintReadWrite, Category = "Selection")
	TObjectPtr<AActor> SelectedBuilding;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	bool bIsDragging = false;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	FVector2D DragStart;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	FVector2D DragEnd;

	// ---- Camera ----
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float PanSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float ZoomSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float MinZoom = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float MaxZoom = 3000.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Camera")
	float CurrentZoom = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float EdgePanMargin = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float EdgePanSpeed = 3000.0f;

	// ---- Building Placement ----
	UPROPERTY(BlueprintReadWrite, Category = "Placement")
	TSubclassOf<AAoABuilding> PendingBuildingClass;

	UPROPERTY(BlueprintReadWrite, Category = "Placement")
	int32 PendingBuildingEmpireIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Placement")
	bool bIsPlacingBuilding = false;

	UPROPERTY(BlueprintReadWrite, Category = "Placement")
	bool bPlaceMultiple = false;

	// ---- Input ----
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	// ---- Selection Functions ----
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void StartDragSelection(FVector2D ScreenPos);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void UpdateDragSelection(FVector2D ScreenPos);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void FinishDragSelection();

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SelectAllMilitary();

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SelectAllVillagers();

	// ---- Commands (client -> server RPCs) ----
	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueMoveCommand(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueAttackTargetCommand(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueAttackGroundCommand(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueGatherCommand(AAoAResourceNode* Resource);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueGatherNearestCommand(EResourceType ResType);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueStopCommand();

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueBuildCommand(TSubclassOf<AAoABuilding> BuildingClass, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueTrainCommand(AAoABuilding* Building, TSubclassOf<AAoAUnit> UnitClass);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueRallyPointCommand(AAoABuilding* Building, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void IssueAgeUpCommand();

	UFUNCTION(BlueprintCallable, Category = "Commands")
	void RequestEmpireChange(int32 EmpireIndex);

protected:
	// Input handlers
	void OnLeftMousePressed();
	void OnLeftMouseReleased();
	void OnRightMousePressed();
	void OnScroll(float Delta);
	void OnPanForward(float Val);
	void OnPanRight(float Val);
	void OnZoomIn();
	void OnZoomOut();

	// Camera helpers
	void UpdateCamera(float DeltaTime);
	void EdgePan(float DeltaTime);

	// Helper to check ownership
	bool OwnsUnit(class AAoAUnit* Unit) const;
	bool OwnsBuilding(class AAoABuilding* Bld) const;

	// Helper to get IDs of selected units owned by this player
	TArray<uint32> GetSelectedUnitIDs() const;

	// Command context: determine what action a right-click should produce
	ECommandType DetermineRightClickAction(const FVector& WorldLoc, AActor* HitActor);

	// ---- Server-side command execution (reliable RPCs) ----
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(const TArray<uint32>& UnitIDs, FVector Location);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttackTarget(const TArray<uint32>& UnitIDs, AActor* Target);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttackGround(const TArray<uint32>& UnitIDs, FVector Location);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Gather(const TArray<uint32>& UnitIDs, AActor* Resource);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Stop(const TArray<uint32>& UnitIDs);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Build(const TArray<uint32>& UnitIDs, TSubclassOf<AAoABuilding> BuildingClass, FVector Location);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Train(AAoABuilding* Building, TSubclassOf<AAoAUnit> UnitClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetRally(AAoABuilding* Building, FVector Location);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AgeUp();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetEmpire(int32 EmpireIndex);
};
