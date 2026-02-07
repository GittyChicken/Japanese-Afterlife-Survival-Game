// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Combat/YomiCombatComponent.h"
#include "Combat/YomiWeaponBase.h"
#include "Character/YomiPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UYomiCombatComponent::UYomiCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UYomiCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPlayer = Cast<AYomiPlayerCharacter>(GetOwner());
}

void UYomiCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Attack cooldown
	if (AttackCooldownTimer > 0.0f)
	{
		AttackCooldownTimer -= DeltaTime;
		if (AttackCooldownTimer <= 0.0f)
		{
			EndAttack();
		}
	}

	// Parry window timer
	if (bInParryWindow)
	{
		ParryTimer -= DeltaTime;
		if (ParryTimer <= 0.0f)
		{
			bInParryWindow = false;
		}
	}

	// Lock-on rotation
	if (LockedTarget)
	{
		if (!LockedTarget->IsValidLowLevel() || LockedTarget->IsActorBeingDestroyed())
		{
			LockedTarget = nullptr;
		}
		else
		{
			UpdateLockOnRotation(DeltaTime);
		}
	}
}

// ============================================================================
// WEAPON MANAGEMENT
// ============================================================================

bool UYomiCombatComponent::EquipWeapon(AYomiWeaponBase* Weapon)
{
	if (!Weapon || !OwnerPlayer) return false;

	if (EquippedWeapon)
	{
		UnequipWeapon();
	}

	EquippedWeapon = Weapon;
	EquippedWeapon->OnEquipped(OwnerPlayer);

	UE_LOG(LogYomiCombat, Log, TEXT("Equipped: %s"), *Weapon->GetWeaponData().DisplayName.ToString());
	return true;
}

void UYomiCombatComponent::UnequipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->OnUnequipped();
		EquippedWeapon = nullptr;
	}
}

// ============================================================================
// ATTACKS
// ============================================================================

void UYomiCombatComponent::ExecuteLightAttack()
{
	if (bIsAttacking || bIsBlocking || !EquippedWeapon || EquippedWeapon->IsBroken()) return;
	if (!OwnerPlayer) return;

	float StaminaCost = EquippedWeapon->GetWeaponData().StaminaCost;
	if (!OwnerPlayer->ConsumeStamina(StaminaCost)) return;

	bIsAttacking = true;
	AttackCooldownTimer = 1.0f / EquippedWeapon->GetWeaponData().AttackSpeed;

	EquippedWeapon->StartLightAttack();
	OnAttackStarted.Broadcast();

	UE_LOG(LogYomiCombat, Verbose, TEXT("Light attack executed"));
}

void UYomiCombatComponent::ExecuteHeavyAttack()
{
	if (bIsAttacking || bIsBlocking || !EquippedWeapon || EquippedWeapon->IsBroken()) return;
	if (!OwnerPlayer) return;

	float StaminaCost = EquippedWeapon->GetWeaponData().StaminaCost * 1.5f;
	if (!OwnerPlayer->ConsumeStamina(StaminaCost)) return;

	bIsAttacking = true;
	AttackCooldownTimer = (1.0f / EquippedWeapon->GetWeaponData().AttackSpeed) * 1.5f;

	EquippedWeapon->StartHeavyAttack();
	OnAttackStarted.Broadcast();

	UE_LOG(LogYomiCombat, Verbose, TEXT("Heavy attack executed"));
}

void UYomiCombatComponent::ExecuteSpecialAttack()
{
	if (bIsAttacking || !EquippedWeapon || EquippedWeapon->IsBroken()) return;
	if (!OwnerPlayer) return;

	float KiCost = EquippedWeapon->GetWeaponData().KiCost;
	if (KiCost > 0.0f && !OwnerPlayer->ConsumeKi(KiCost)) return;

	bIsAttacking = true;
	AttackCooldownTimer = (1.0f / EquippedWeapon->GetWeaponData().AttackSpeed) * 2.0f;

	EquippedWeapon->StartSpecialAttack();
	OnAttackStarted.Broadcast();

	UE_LOG(LogYomiCombat, Log, TEXT("Special attack executed (Ki cost: %f)"), KiCost);
}

void UYomiCombatComponent::EndAttack()
{
	bIsAttacking = false;
	if (EquippedWeapon)
	{
		EquippedWeapon->DisableDamageCollision();
	}
	OnAttackEnded.Broadcast();
}

// ============================================================================
// BLOCKING & PARRYING
// ============================================================================

void UYomiCombatComponent::StartBlocking()
{
	if (bIsAttacking || !EquippedWeapon) return;
	if (!EquippedWeapon->GetWeaponData().bCanBlock) return;

	bIsBlocking = true;
	bInParryWindow = true;
	ParryTimer = ParryWindowDuration;

	EquippedWeapon->StartBlock();
}

void UYomiCombatComponent::StopBlocking()
{
	bIsBlocking = false;
	bInParryWindow = false;

	if (EquippedWeapon)
	{
		EquippedWeapon->EndBlock();
	}
}

float UYomiCombatComponent::ProcessBlockedDamage(float IncomingDamage, EDamageType DamageType)
{
	if (!bIsBlocking || !EquippedWeapon) return IncomingDamage;

	// Perfect parry - negate all damage and stagger attacker
	if (bInParryWindow)
	{
		OnParrySuccessful.Broadcast(nullptr);
		UE_LOG(LogYomiCombat, Log, TEXT("Perfect parry!"));

		if (OwnerPlayer)
		{
			OwnerPlayer->AddHonor(2.0f);
		}
		return 0.0f;
	}

	// Regular block
	if (OwnerPlayer)
	{
		OwnerPlayer->ConsumeStamina(BlockStaminaCostPerHit);
	}

	float BlockReduction = EquippedWeapon->GetWeaponData().BlockDamageReduction;
	return IncomingDamage * (1.0f - BlockReduction);
}

// ============================================================================
// LOCK-ON
// ============================================================================

void UYomiCombatComponent::ToggleLockOn()
{
	if (LockedTarget)
	{
		LockedTarget = nullptr;
		return;
	}

	LockedTarget = FindLockOnTarget();
}

AActor* UYomiCombatComponent::FindLockOnTarget() const
{
	if (!OwnerPlayer) return nullptr;

	FVector Start = OwnerPlayer->GetActorLocation();
	TArray<FHitResult> HitResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnRange);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPlayer);

	if (GetWorld()->SweepMultiByChannel(HitResults, Start, Start, FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		AActor* ClosestTarget = nullptr;
		float ClosestDist = LockOnRange;

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == OwnerPlayer) continue;

			// Check if it's an enemy
			AYomiCharacterBase* Character = Cast<AYomiCharacterBase>(HitActor);
			if (!Character || !Character->IsAlive()) continue;

			float Dist = FVector::Dist(Start, HitActor->GetActorLocation());
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				ClosestTarget = HitActor;
			}
		}

		return ClosestTarget;
	}

	return nullptr;
}

void UYomiCombatComponent::UpdateLockOnRotation(float DeltaTime)
{
	if (!OwnerPlayer || !LockedTarget) return;

	FVector Direction = LockedTarget->GetActorLocation() - OwnerPlayer->GetActorLocation();
	Direction.Z = 0.0f;
	FRotator TargetRotation = Direction.Rotation();

	FRotator CurrentRotation = OwnerPlayer->GetControlRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
	OwnerPlayer->GetController()->SetControlRotation(NewRotation);
}

// ============================================================================
// KI ABILITIES
// ============================================================================

void UYomiCombatComponent::KiDash()
{
	if (!OwnerPlayer || !OwnerPlayer->ConsumeKi(KiDashCost)) return;

	FVector DashDirection = OwnerPlayer->GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = OwnerPlayer->GetActorForwardVector();
	}

	OwnerPlayer->LaunchCharacter(DashDirection * 1500.0f, true, false);
	UE_LOG(LogYomiCombat, Log, TEXT("Ki Dash!"));
}

void UYomiCombatComponent::KiPowerStrike()
{
	if (!OwnerPlayer || !EquippedWeapon) return;
	if (!OwnerPlayer->ConsumeKi(KiPowerStrikeCost)) return;

	// Enhanced damage attack with spirit energy
	bIsAttacking = true;
	AttackCooldownTimer = 1.0f;

	EquippedWeapon->StartSpecialAttack();
	OnAttackStarted.Broadcast();

	UE_LOG(LogYomiCombat, Log, TEXT("Ki Power Strike!"));
}

void UYomiCombatComponent::KiSpiritArrow()
{
	if (!OwnerPlayer) return;
	if (!OwnerPlayer->ConsumeKi(KiSpiritArrowCost)) return;

	// In full implementation, this would spawn a spirit projectile
	FVector SpawnLocation = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = OwnerPlayer->GetControlRotation();

	UE_LOG(LogYomiCombat, Log, TEXT("Ki Spirit Arrow fired from %s"), *SpawnLocation.ToString());
}
