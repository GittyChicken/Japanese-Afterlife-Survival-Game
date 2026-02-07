// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/YomiGameTypes.h"
#include "YomiCombatComponent.generated.h"

class AYomiWeaponBase;
class AYomiPlayerCharacter;

/**
 * Combat component handling weapon management, attack execution,
 * blocking, parrying, and the Ki-powered special ability system.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOMISURVIVAL_API UYomiCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UYomiCombatComponent();

	// ========================================================================
	// WEAPON MANAGEMENT
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool EquipWeapon(AYomiWeaponBase* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UnequipWeapon();

	UFUNCTION(BlueprintPure, Category = "Combat")
	AYomiWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool HasWeaponEquipped() const { return EquippedWeapon != nullptr; }

	// ========================================================================
	// ATTACKS
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteSpecialAttack();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const { return bIsAttacking; }

	// ========================================================================
	// BLOCKING & PARRYING
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartBlocking();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopBlocking();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsBlocking() const { return bIsBlocking; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsParryWindow() const { return bInParryWindow; }

	/** Attempt to block incoming damage. Returns remaining damage after block. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float ProcessBlockedDamage(float IncomingDamage, EDamageType DamageType);

	// ========================================================================
	// LOCK-ON SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleLockOn();

	UFUNCTION(BlueprintPure, Category = "Combat")
	AActor* GetLockedTarget() const { return LockedTarget; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsLockedOn() const { return LockedTarget != nullptr; }

	// ========================================================================
	// KI ABILITIES
	// ========================================================================

	/** Ki Dash - Quick dash with i-frames, costs Ki */
	UFUNCTION(BlueprintCallable, Category = "Combat|Ki")
	void KiDash();

	/** Ki Power Strike - Enhanced melee attack with spirit energy */
	UFUNCTION(BlueprintCallable, Category = "Combat|Ki")
	void KiPowerStrike();

	/** Ki Spirit Arrow - Ranged spirit projectile */
	UFUNCTION(BlueprintCallable, Category = "Combat|Ki")
	void KiSpiritArrow();

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParrySuccessful, AActor*, ParriedActor);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackStarted OnAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackEnded OnAttackEnded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnParrySuccessful OnParrySuccessful;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<AYomiWeaponBase> EquippedWeapon;

	UPROPERTY()
	TObjectPtr<AYomiPlayerCharacter> OwnerPlayer;

	UPROPERTY()
	TObjectPtr<AActor> LockedTarget;

	// Attack state
	bool bIsAttacking = false;
	float AttackCooldownTimer = 0.0f;

	// Blocking
	bool bIsBlocking = false;
	bool bInParryWindow = false;
	float ParryWindowDuration = 0.2f;
	float ParryTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float BlockStaminaCostPerHit = 15.0f;

	// Ki costs
	UPROPERTY(EditAnywhere, Category = "Combat|Ki")
	float KiDashCost = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Ki")
	float KiPowerStrikeCost = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Ki")
	float KiSpiritArrowCost = 20.0f;

	// Lock-on
	UPROPERTY(EditAnywhere, Category = "Combat")
	float LockOnRange = 1500.0f;

	AActor* FindLockOnTarget() const;
	void UpdateLockOnRotation(float DeltaTime);

	void EndAttack();
};
