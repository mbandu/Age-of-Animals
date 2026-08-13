#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoAPlayerState.h"
#include "AoAResourceNode.generated.h"

/**
 * A gatherable resource node on the map. Forest tiles yield wood, bamboo
 * tiles yield food, rock tiles yield stone. Each node has a finite amount.
 */
UCLASS(BlueprintType)
class AAoAResourceNode : public AActor
{
	GENERATED_BODY()

public:
	AAoAResourceNode();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
	EResourceType ResourceType = EResourceType::Wood;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
	int32 RemainingAmount = 400;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
	int32 MaxAmount = 400;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UStaticMesh* TreeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UStaticMesh* BambooMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UStaticMesh* RockMesh;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void Harvest(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 GetRemainingAmount() const { return RemainingAmount; }

	UFUNCTION(BlueprintCallable, Category = "Resource")
	EResourceType GetResourceType() const { return ResourceType; }

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsDepleted() const { return RemainingAmount <= 0; }

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void Initialize(EResourceType Type, int32 Amount);
};
