// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/YomiCharacterBase.h"
#include "Core/YomiGameTypes.h"
#include "YomiEnemyBase.generated.h"

class UBehaviorTree;
class AYomiAIController;

/**
 * Base class for all enemies in Yomi Survival.
 * Yokai, Oni, spirits, bandits, corrupted creatures, etc.
 * Each enemy has attack patterns, loot tables, and biome affinity.
 */
UCLASS()
class YOMISURVIVAL_API AYomiEnemyBase : public AYomiCharacterBase
{
	GENERATED_BODY()

public:
	AYomiEnemyBase();

	UFUNCTION(BlueprintPure, Category = "Enemy")
	EEnemyType GetEnemyType() const { return EnemyType; }

	UFUNCTION(BlueprintPure, Category = "Enemy")
	EYomiBiome GetHomeBiome() const { return HomeBiome; }

	UFUNCTION(BlueprintPure, Category = "Enemy")
	int32 GetDifficultyRating() const { return DifficultyRating; }

	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsAggressive() const { return bIsAggressive; }

	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetAggroRange() const { return AggroRange; }

	/** Called when this enemy detects a player. */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	virtual void OnPlayerDetected(AActor* Player);

	/** Called when this enemy loses sight of the player. */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	virtual void OnPlayerLost();

	/** Execute the enemy's primary attack. */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	virtual void ExecuteAttack();

	/** Execute a special ability. */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	virtual void ExecuteSpecialAbility();

	/** Get loot to drop on death. */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Loot")
	virtual TMap<FName, int32> GetLootDrop() const;

	/** Whether this kill counts as honorable (1v1, face-to-face). */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool WasKilledHonorably() const { return bKilledHonorably; }

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetKilledHonorably(bool bHonorable) { bKilledHonorably = bHonorable; }

protected:
	virtual void BeginPlay() override;
	virtual void Die() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyType EnemyType = EEnemyType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EYomiBiome HomeBiome = EYomiBiome::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int32 DifficultyRating = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	bool bIsAggressive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AggroRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackDamage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EDamageType AttackDamageType = EDamageType::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float PatrolRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// Loot table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	TMap<FName, int32> GuaranteedLoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	TMap<FName, float> ChanceLoot; // FName -> drop chance (0.0 - 1.0)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	int32 HonorReward = 5;

	// Combat state
	float AttackCooldownTimer = 0.0f;
	bool bKilledHonorably = false;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
};
