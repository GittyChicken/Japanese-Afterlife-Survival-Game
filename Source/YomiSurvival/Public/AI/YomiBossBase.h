// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/YomiEnemyBase.h"
#include "YomiBossBase.generated.h"

/**
 * Base class for all boss encounters in Yomi Survival.
 * Bosses have multiple phases, unique mechanics, and Bushido challenge support.
 *
 * Bosses:
 * 1. Kitsune no Ōkami - Illusions, teleportation (teaches dodging)
 * 2. Mizuchi - Water attacks, flooding arena (teaches adaptability)
 * 3. Daitengu - Flying, wind attacks (teaches patience/timing)
 * 4. Ōmukade - Poison, segmented weak points (teaches precision)
 * 5. Shuten-dōji - Berserker rage, fire breath (teaches composure)
 * 6. Gashadokuro Titan - Multi-phase, summons adds (skill check)
 * 7. Yamata no Orochi - 8 heads/phases, each different element (final test)
 */
UCLASS()
class YOMISURVIVAL_API AYomiBossBase : public AYomiEnemyBase
{
	GENERATED_BODY()

public:
	AYomiBossBase();

	// ========================================================================
	// BOSS IDENTITY
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Boss")
	EBossType GetBossType() const { return BossType; }

	UFUNCTION(BlueprintPure, Category = "Boss")
	FText GetBossTitle() const { return BossTitle; }

	// ========================================================================
	// PHASE SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	int32 GetTotalPhases() const { return TotalPhases; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	virtual void TransitionToPhase(int32 NewPhase);

	/** Called when health drops below a phase threshold. */
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	virtual void CheckPhaseTransition();

	// ========================================================================
	// BOSS MECHANICS
	// ========================================================================

	/** Start the boss encounter. */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void StartEncounter(AActor* Challenger);

	/** End the boss encounter (on defeat or player death). */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void EndEncounter(bool bPlayerWon);

	/** Summon minions (specific bosses). */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void SummonMinions();

	/** Execute a phase-specific attack pattern. */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void ExecutePhaseAttack();

	/** Enrage mechanic - boss gets stronger over time or at low HP. */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void Enrage();

	UFUNCTION(BlueprintPure, Category = "Boss")
	bool IsEnraged() const { return bIsEnraged; }

	UFUNCTION(BlueprintPure, Category = "Boss")
	bool IsEncounterActive() const { return bEncounterActive; }

	// ========================================================================
	// ARENA
	// ========================================================================

	/** Arena boundary - prevents player from leaving during boss fight. */
	UFUNCTION(BlueprintCallable, Category = "Boss|Arena")
	void ActivateArena();

	UFUNCTION(BlueprintCallable, Category = "Boss|Arena")
	void DeactivateArena();

	// ========================================================================
	// BUSHIDO CHALLENGE TRACKING
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Boss|Bushido")
	void SetActiveBushidoChallenge(EBushidoChallenge Challenge) { ActiveBushidoChallenge = Challenge; }

	UFUNCTION(BlueprintPure, Category = "Boss|Bushido")
	EBushidoChallenge GetActiveBushidoChallenge() const { return ActiveBushidoChallenge; }

	/** Track challenge conditions (damage taken, minions killed, etc.) */
	UFUNCTION(BlueprintCallable, Category = "Boss|Bushido")
	void OnPlayerTookDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Boss|Bushido")
	void OnMinionKilled();

	UFUNCTION(BlueprintPure, Category = "Boss|Bushido")
	bool IsBushidoChallengeComplete() const;

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossEnraged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterEnded, bool, bPlayerWon);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBossEnraged OnBossEnraged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEncounterEnded OnEncounterEnded;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Die() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	EBossType BossType = EBossType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FText BossTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	int32 TotalPhases = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	TArray<float> PhaseHealthThresholds; // e.g., {0.75, 0.5, 0.25}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float EnrageHealthThreshold = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float EnrageDamageMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float EnrageSpeedMultiplier = 1.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Arena")
	float ArenaRadius = 2000.0f;

private:
	int32 CurrentPhase = 0;
	bool bIsEnraged = false;
	bool bEncounterActive = false;
	float EncounterTimer = 0.0f;

	EBushidoChallenge ActiveBushidoChallenge = EBushidoChallenge::None;
	float TotalDamageTakenByPlayer = 0.0f;
	int32 MinionsKilledByPlayer = 0;

	UPROPERTY()
	TObjectPtr<AActor> Challenger;
};
