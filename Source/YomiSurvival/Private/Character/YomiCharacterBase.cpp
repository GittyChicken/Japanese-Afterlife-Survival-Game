// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Character/YomiCharacterBase.h"
#include "Net/UnrealNetwork.h"

AYomiCharacterBase::AYomiCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}

void AYomiCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}

void AYomiCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	// Health regeneration
	if (CurrentHealth < MaxHealth && HealthRegenRate > 0.0f)
	{
		Heal(HealthRegenRate * DeltaTime);
	}

	// Stamina regeneration with delay
	if (StaminaRegenTimer > 0.0f)
	{
		StaminaRegenTimer -= DeltaTime;
	}
	else if (CurrentStamina < MaxStamina)
	{
		RestoreStamina(StaminaRegenRate * DeltaTime);
	}
}

void AYomiCharacterBase::Heal(float Amount)
{
	if (Amount <= 0.0f || !IsAlive()) return;

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AYomiCharacterBase::RestoreStamina(float Amount)
{
	if (Amount <= 0.0f) return;

	CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

bool AYomiCharacterBase::ConsumeStamina(float Amount)
{
	if (CurrentStamina < Amount) return false;

	CurrentStamina = FMath::Max(0.0f, CurrentStamina - Amount);
	StaminaRegenTimer = StaminaRegenDelay;
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	return true;
}

float AYomiCharacterBase::ApplyDamage(float DamageAmount, EDamageType DamageType, AActor* DamageCauser)
{
	if (!IsAlive() || DamageAmount <= 0.0f) return 0.0f;

	float Resistance = GetDamageResistance(DamageType);
	float FinalDamage = DamageAmount * (1.0f - Resistance);
	FinalDamage = FMath::Max(0.0f, FinalDamage);

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - FinalDamage);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDamageTaken.Broadcast(FinalDamage, DamageType, DamageCauser);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return FinalDamage;
}

float AYomiCharacterBase::GetDamageResistance(EDamageType DamageType) const
{
	if (const float* Resistance = DamageResistances.Find(DamageType))
	{
		return FMath::Clamp(*Resistance, 0.0f, 0.9f); // Cap at 90% resistance
	}
	return 0.0f;
}

void AYomiCharacterBase::Die()
{
	OnDeath.Broadcast();
	UE_LOG(LogYomi, Log, TEXT("%s has died."), *GetName());
}

void AYomiCharacterBase::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AYomiCharacterBase::OnRep_CurrentStamina()
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void AYomiCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYomiCharacterBase, CurrentHealth);
	DOREPLIFETIME(AYomiCharacterBase, MaxHealth);
	DOREPLIFETIME(AYomiCharacterBase, CurrentStamina);
	DOREPLIFETIME(AYomiCharacterBase, MaxStamina);
}
