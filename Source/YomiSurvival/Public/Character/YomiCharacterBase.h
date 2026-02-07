// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/YomiGameTypes.h"
#include "YomiCharacterBase.generated.h"

class UYomiCombatComponent;

/**
 * Base character class for all characters in Yomi Survival.
 * Provides health, stamina, damage handling, and status effects.
 */
UCLASS(Abstract)
class YOMISURVIVAL_API AYomiCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AYomiCharacterBase();

	// ========================================================================
	// HEALTH & STAMINA
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStaminaPercent() const { return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	bool IsAlive() const { return CurrentHealth > 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void RestoreStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool ConsumeStamina(float Amount);

	// ========================================================================
	// DAMAGE
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float ApplyDamage(float DamageAmount, EDamageType DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintPure, Category = "Combat")
	virtual float GetDamageResistance(EDamageType DamageType) const;

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTaken, float, DamageAmount, EDamageType, DamageType, AActor*, DamageCauser);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamageTaken OnDamageTaken;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChanged OnStaminaChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Die();

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", Replicated)
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", ReplicatedUsing = OnRep_CurrentStamina)
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", Replicated)
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HealthRegenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float StaminaRegenRate = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float StaminaRegenDelay = 1.0f;

	float StaminaRegenTimer = 0.0f;

	// Damage resistances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<EDamageType, float> DamageResistances;

	// Replication
	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_CurrentStamina();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
