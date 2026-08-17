#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AoAEmpireData.h"
#include "AoAPlayerController.generated.h"

UCLASS(BlueprintType)
class AAoAPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAoAPlayerController();
	UPROPERTY(BlueprintReadWrite, Category = "Selection") TArray<TObjectPtr<AActor>> SelectedActors;
	UPROPERTY(BlueprintReadWrite, Category = "Camera") float CurrentZoom = 1500.0f;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	void OnLeftClick();
	void OnRightClick();
	UFUNCTION(BlueprintCallable, Category = "Commands") void IssueMoveCommand(const FVector& Location);
};
