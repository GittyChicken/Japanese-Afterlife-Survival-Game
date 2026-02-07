// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "AI/YomiEnemyBase.h"
#include "Character/YomiPlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

AYomiEnemyBase::AYomiEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default enemy stats
	MaxHealth = 50.0f;
	MaxStamina = 50.0f;
	HealthRegenRate = 0.0f;
}

void AYomiEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AYomiEnemyBase::OnPlayerDetected(AActor* Player)
{
	CurrentTarget = Player;
	UE_LOG(LogYomiAI, Log, TEXT("%s detected player: %s"), *GetName(), *Player->GetName());
}

void AYomiEnemyBase::OnPlayerLost()
{
	CurrentTarget = nullptr;
	UE_LOG(LogYomiAI, Log, TEXT("%s lost sight of player"), *GetName());
}

void AYomiEnemyBase::ExecuteAttack()
{
	if (!CurrentTarget || !IsAlive()) return;

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > AttackRange) return;

	AYomiCharacterBase* TargetCharacter = Cast<AYomiCharacterBase>(CurrentTarget);
	if (TargetCharacter)
	{
		TargetCharacter->ApplyDamage(AttackDamage, AttackDamageType, this);
		UE_LOG(LogYomiAI, Verbose, TEXT("%s attacked %s for %f damage"),
			*GetName(), *CurrentTarget->GetName(), AttackDamage);
	}
}

void AYomiEnemyBase::ExecuteSpecialAbility()
{
	// Override in subclasses for unique yokai abilities
	UE_LOG(LogYomiAI, Log, TEXT("%s used special ability"), *GetName());
}

TMap<FName, int32> AYomiEnemyBase::GetLootDrop() const
{
	TMap<FName, int32> Loot = GuaranteedLoot;

	for (const auto& Pair : ChanceLoot)
	{
		if (FMath::FRand() <= Pair.Value)
		{
			if (int32* Existing = Loot.Find(Pair.Key))
			{
				(*Existing)++;
			}
			else
			{
				Loot.Add(Pair.Key, 1);
			}
		}
	}

	return Loot;
}

void AYomiEnemyBase::Die()
{
	// Drop loot
	TMap<FName, int32> Loot = GetLootDrop();
	for (const auto& Pair : Loot)
	{
		UE_LOG(LogYomiAI, Log, TEXT("Dropped loot: %s x%d"), *Pair.Key.ToString(), Pair.Value);
		// In full implementation, spawn world pickup actors
	}

	// Award honor to killer
	if (CurrentTarget)
	{
		AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(CurrentTarget);
		if (Player)
		{
			if (bKilledHonorably)
			{
				Player->OnHonorableKill(this);
			}
			else
			{
				Player->AddHonor(static_cast<float>(HonorReward));
			}
		}
	}

	Super::Die();

	// Destroy after delay for death animation
	SetLifeSpan(5.0f);
}
