// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "AI/YomiBossBase.h"
#include "Character/YomiPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AYomiBossBase::AYomiBossBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Boss default stats - much stronger than regular enemies
	MaxHealth = 1000.0f;
	MaxStamina = 200.0f;
	AttackDamage = 40.0f;
	HonorReward = 50;
	DifficultyRating = 10;

	// Default phase thresholds
	PhaseHealthThresholds = {0.75f, 0.5f, 0.25f};
}

void AYomiBossBase::BeginPlay()
{
	Super::BeginPlay();
}

void AYomiBossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEncounterActive)
	{
		EncounterTimer += DeltaTime;
		CheckPhaseTransition();
	}
}

// ============================================================================
// ENCOUNTER
// ============================================================================

void AYomiBossBase::StartEncounter(AActor* InChallenger)
{
	Challenger = InChallenger;
	bEncounterActive = true;
	CurrentPhase = 0;
	EncounterTimer = 0.0f;
	TotalDamageTakenByPlayer = 0.0f;
	MinionsKilledByPlayer = 0;

	ActivateArena();
	OnPlayerDetected(InChallenger);

	UE_LOG(LogYomiAI, Log, TEXT("Boss encounter started: %s vs %s"),
		*BossTitle.ToString(), *InChallenger->GetName());
}

void AYomiBossBase::EndEncounter(bool bPlayerWon)
{
	bEncounterActive = false;
	DeactivateArena();

	if (bPlayerWon)
	{
		AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(Challenger);
		if (Player)
		{
			Player->OnBossDefeated(BossType);

			// Check Bushido challenge completion
			if (ActiveBushidoChallenge != EBushidoChallenge::None && IsBushidoChallengeComplete())
			{
				UE_LOG(LogYomiAI, Log, TEXT("Bushido Challenge completed!"));
				Player->AddHonor(100.0f);
			}
		}
	}

	OnEncounterEnded.Broadcast(bPlayerWon);
	UE_LOG(LogYomiAI, Log, TEXT("Boss encounter ended. Player won: %s"), bPlayerWon ? TEXT("Yes") : TEXT("No"));
}

// ============================================================================
// PHASE SYSTEM
// ============================================================================

void AYomiBossBase::CheckPhaseTransition()
{
	float HealthPercent = GetHealthPercent();

	// Check if we should transition to next phase
	if (CurrentPhase < PhaseHealthThresholds.Num())
	{
		if (HealthPercent <= PhaseHealthThresholds[CurrentPhase])
		{
			TransitionToPhase(CurrentPhase + 1);
		}
	}

	// Check enrage
	if (!bIsEnraged && HealthPercent <= EnrageHealthThreshold)
	{
		Enrage();
	}
}

void AYomiBossBase::TransitionToPhase(int32 NewPhase)
{
	if (NewPhase == CurrentPhase) return;

	CurrentPhase = NewPhase;
	OnPhaseChanged.Broadcast(CurrentPhase);

	UE_LOG(LogYomiAI, Log, TEXT("Boss %s transitioned to phase %d/%d"),
		*BossTitle.ToString(), CurrentPhase + 1, TotalPhases);

	// Override in subclasses for phase-specific behavior changes
}

void AYomiBossBase::ExecutePhaseAttack()
{
	// Override in boss subclasses for unique attack patterns per phase
	ExecuteAttack();
}

// ============================================================================
// MECHANICS
// ============================================================================

void AYomiBossBase::SummonMinions()
{
	// Override in specific bosses (e.g., Gashadokuro Titan)
	UE_LOG(LogYomiAI, Log, TEXT("Boss %s summoning minions"), *BossTitle.ToString());
}

void AYomiBossBase::Enrage()
{
	bIsEnraged = true;
	AttackDamage *= EnrageDamageMultiplier;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= EnrageSpeedMultiplier;
	}

	OnBossEnraged.Broadcast();
	UE_LOG(LogYomiAI, Log, TEXT("Boss %s has ENRAGED!"), *BossTitle.ToString());
}

void AYomiBossBase::ActivateArena()
{
	// In full implementation, spawn arena boundary walls/barriers
	UE_LOG(LogYomiAI, Log, TEXT("Boss arena activated (radius: %f)"), ArenaRadius);
}

void AYomiBossBase::DeactivateArena()
{
	UE_LOG(LogYomiAI, Log, TEXT("Boss arena deactivated"));
}

// ============================================================================
// BUSHIDO CHALLENGE
// ============================================================================

void AYomiBossBase::OnPlayerTookDamage(float DamageAmount)
{
	TotalDamageTakenByPlayer += DamageAmount;
}

void AYomiBossBase::OnMinionKilled()
{
	MinionsKilledByPlayer++;
}

bool AYomiBossBase::IsBushidoChallengeComplete() const
{
	switch (ActiveBushidoChallenge)
	{
	case EBushidoChallenge::PerfectDodge:
		return TotalDamageTakenByPlayer <= 0.0f;
	case EBushidoChallenge::Pacifist:
		return MinionsKilledByPlayer == 0;
	case EBushidoChallenge::SpeedRun:
		return EncounterTimer <= 120.0f; // 2 minutes
	case EBushidoChallenge::NoArmor:
	case EBushidoChallenge::KatanaOnly:
	case EBushidoChallenge::NoHealing:
		return true; // These are tracked by the player character's constraints
	default:
		return false;
	}
}

// ============================================================================
// DEATH
// ============================================================================

void AYomiBossBase::Die()
{
	if (bEncounterActive)
	{
		EndEncounter(true);
	}

	Super::Die();
}
