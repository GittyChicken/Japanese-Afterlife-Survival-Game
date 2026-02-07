// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiWeaponBase.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;

/**
 * Base class for all weapons in Yomi Survival.
 * Handles damage calculation, durability, combo tracking, and spirit effects.
 */
UCLASS(Abstract)
class YOMISURVIVAL_API AYomiWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AYomiWeaponBase();

	// ========================================================================
	// WEAPON DATA
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Weapon")
	const FWeaponData& GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InitializeWeapon(const FWeaponData& InData);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetWeaponType() const { return WeaponData.WeaponType; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponTier GetWeaponTier() const { return WeaponData.Tier; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponClass GetWeaponClass() const { return WeaponData.WeaponClass; }

	// ========================================================================
	// COMBAT
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartSpecialAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableDamageCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableDamageCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndBlock();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsBlocking() const { return bIsBlocking; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	float CalculateDamage(bool bIsHeavyAttack, bool bIsStealthAttack) const;

	// ========================================================================
	// COMBO SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetCurrentCombo() const { return CurrentComboCount; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void IncrementCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();

	// ========================================================================
	// DURABILITY
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentDurability() const { return CurrentDurability; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetDurabilityPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReduceDurability(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RepairWeapon(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsBroken() const { return CurrentDurability <= 0; }

	// ========================================================================
	// EQUIP / UNEQUIP
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void OnEquipped(ACharacter* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void OnUnequipped();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ACharacter* GetWeaponOwner() const { return WeaponOwner; }

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponHit, AActor*, HitActor, float, DamageDealt, const FHitResult&, HitResult);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponBroken, AYomiWeaponBase*, BrokenWeapon);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponHit OnWeaponHit;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponBroken OnWeaponBroken;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDamageBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> DamageCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> SpiritEffectComponent;

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponData WeaponData;

	UPROPERTY()
	TObjectPtr<ACharacter> WeaponOwner;

	// State
	int32 CurrentDurability;
	int32 CurrentComboCount = 0;
	int32 MaxCombo = 3;
	float ComboTimer = 0.0f;
	float ComboWindowDuration = 1.5f;
	bool bIsBlocking = false;
	bool bDamageEnabled = false;

	// Track what we've hit this swing to avoid double-hits
	TArray<AActor*> HitActorsThisSwing;

	// Combo damage multiplier
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ComboMultiplierPerHit = 0.15f;

	// Heavy attack multiplier
	UPROPERTY(EditAnywhere, Category = "Combat")
	float HeavyAttackMultiplier = 2.0f;
};
