// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiResourceNode.generated.h"

class UStaticMeshComponent;

/**
 * A harvestable resource node in the world (trees, rocks, ore deposits, plants).
 */
UCLASS()
class YOMISURVIVAL_API AYomiResourceNode : public AActor
{
	GENERATED_BODY()

public:
	AYomiResourceNode();

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void InitializeResource(EResourceType InType, int32 InAmount = -1);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 Harvest(AActor* Harvester, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "Resource")
	EResourceType GetResourceType() const { return ResourceType; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	int32 GetRemainingAmount() const { return RemainingAmount; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	bool IsDepleted() const { return RemainingAmount <= 0; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EResourceType ResourceType = EResourceType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 MaxAmount = 10;

	UPROPERTY(BlueprintReadOnly, Category = "Resource", Replicated)
	int32 RemainingAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float RespawnTime = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	bool bCanRespawn = true;

	void OnDepleted();
	void Respawn();

	FTimerHandle RespawnTimerHandle;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
