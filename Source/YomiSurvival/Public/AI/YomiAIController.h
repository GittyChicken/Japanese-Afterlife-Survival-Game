// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "YomiAIController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

/**
 * AI Controller for enemies and companions in Yomi Survival.
 * Uses behavior trees with perception (sight, hearing) for
 * patrol, chase, attack, and retreat behaviors.
 */
UCLASS()
class YOMISURVIVAL_API AYomiAIController : public AAIController
{
	GENERATED_BODY()

public:
	AYomiAIController();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetPatrolCenter(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetPatrolCenter() const { return PatrolCenter; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetPatrolRadius() const { return PatrolRadius; }

	/** Get a random patrol point within the patrol radius. */
	UFUNCTION(BlueprintPure, Category = "AI")
	FVector GetRandomPatrolPoint() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UPROPERTY(EditAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

private:
	FVector PatrolCenter;
	float PatrolRadius = 500.0f;
};
