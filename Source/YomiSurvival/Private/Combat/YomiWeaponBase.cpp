// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Combat/YomiWeaponBase.h"
#include "Character/YomiCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"

AYomiWeaponBase::AYomiWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootSceneComponent);

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(RootSceneComponent);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	DamageCollisionBox->SetupAttachment(WeaponMeshComponent);
	DamageCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageCollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	DamageCollisionBox->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));

	SpiritEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpiritEffect"));
	SpiritEffectComponent->SetupAttachment(WeaponMeshComponent);
	SpiritEffectComponent->bAutoActivate = false;

	CurrentDurability = 100;
}

void AYomiWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AYomiWeaponBase::OnDamageBoxOverlap);
	DisableDamageCollision();
}

void AYomiWeaponBase::InitializeWeapon(const FWeaponData& InData)
{
	WeaponData = InData;
	CurrentDurability = WeaponData.MaxDurability;

	// Enable spirit effects for Tier 4 weapons
	if (WeaponData.Tier == EWeaponTier::Spirit && SpiritEffectComponent)
	{
		SpiritEffectComponent->Activate(true);
	}
}

// ============================================================================
// COMBAT
// ============================================================================

void AYomiWeaponBase::StartLightAttack()
{
	if (IsBroken()) return;
	HitActorsThisSwing.Empty();
	EnableDamageCollision();
}

void AYomiWeaponBase::StartHeavyAttack()
{
	if (IsBroken()) return;
	HitActorsThisSwing.Empty();
	EnableDamageCollision();
}

void AYomiWeaponBase::StartSpecialAttack()
{
	if (IsBroken()) return;
	HitActorsThisSwing.Empty();
	EnableDamageCollision();
}

void AYomiWeaponBase::EnableDamageCollision()
{
	bDamageEnabled = true;
	DamageCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AYomiWeaponBase::DisableDamageCollision()
{
	bDamageEnabled = false;
	DamageCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActorsThisSwing.Empty();
}

void AYomiWeaponBase::StartBlock()
{
	if (WeaponData.bCanBlock)
	{
		bIsBlocking = true;
	}
}

void AYomiWeaponBase::EndBlock()
{
	bIsBlocking = false;
}

float AYomiWeaponBase::CalculateDamage(bool bIsHeavyAttack, bool bIsStealthAttack) const
{
	float Damage = WeaponData.BaseDamage;

	// Heavy attack multiplier
	if (bIsHeavyAttack)
	{
		Damage *= HeavyAttackMultiplier;
	}

	// Combo multiplier
	Damage *= (1.0f + CurrentComboCount * ComboMultiplierPerHit);

	// Stealth bonus
	if (bIsStealthAttack && WeaponData.bHasStealthBonus)
	{
		Damage *= WeaponData.StealthDamageMultiplier;
	}

	// Durability penalty when low
	float DurabilityPercent = GetDurabilityPercent();
	if (DurabilityPercent < 0.2f)
	{
		Damage *= 0.5f; // Half damage when near broken
	}

	return Damage;
}

// ============================================================================
// COMBO
// ============================================================================

void AYomiWeaponBase::IncrementCombo()
{
	CurrentComboCount = FMath::Min(CurrentComboCount + 1, MaxCombo);
	ComboTimer = ComboWindowDuration;
}

void AYomiWeaponBase::ResetCombo()
{
	CurrentComboCount = 0;
	ComboTimer = 0.0f;
}

// ============================================================================
// DURABILITY
// ============================================================================

float AYomiWeaponBase::GetDurabilityPercent() const
{
	return WeaponData.MaxDurability > 0 ? static_cast<float>(CurrentDurability) / WeaponData.MaxDurability : 0.0f;
}

void AYomiWeaponBase::ReduceDurability(int32 Amount)
{
	CurrentDurability = FMath::Max(0, CurrentDurability - Amount);
	if (CurrentDurability <= 0)
	{
		OnWeaponBroken.Broadcast(this);
		UE_LOG(LogYomiCombat, Warning, TEXT("Weapon %s has broken!"), *WeaponData.DisplayName.ToString());
	}
}

void AYomiWeaponBase::RepairWeapon(int32 Amount)
{
	CurrentDurability = FMath::Min(CurrentDurability + Amount, WeaponData.MaxDurability);
}

// ============================================================================
// EQUIP
// ============================================================================

void AYomiWeaponBase::OnEquipped(ACharacter* NewOwner)
{
	WeaponOwner = NewOwner;
	AttachToComponent(NewOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("weapon_r"));
	UE_LOG(LogYomiCombat, Log, TEXT("Weapon %s equipped by %s"),
		*WeaponData.DisplayName.ToString(), *NewOwner->GetName());
}

void AYomiWeaponBase::OnUnequipped()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WeaponOwner = nullptr;
}

// ============================================================================
// COLLISION
// ============================================================================

void AYomiWeaponBase::OnDamageBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bDamageEnabled || !OtherActor || OtherActor == this || OtherActor == WeaponOwner) return;
	if (HitActorsThisSwing.Contains(OtherActor)) return;

	HitActorsThisSwing.Add(OtherActor);

	AYomiCharacterBase* HitCharacter = Cast<AYomiCharacterBase>(OtherActor);
	if (HitCharacter)
	{
		float Damage = CalculateDamage(false, false);
		float DamageDealt = HitCharacter->ApplyDamage(Damage, WeaponData.PrimaryDamageType, WeaponOwner);

		// Apply secondary damage if exists
		if (WeaponData.SecondaryDamageType != EDamageType::None && WeaponData.SecondaryDamageAmount > 0.0f)
		{
			HitCharacter->ApplyDamage(WeaponData.SecondaryDamageAmount, WeaponData.SecondaryDamageType, WeaponOwner);
		}

		OnWeaponHit.Broadcast(OtherActor, DamageDealt, SweepResult);
		ReduceDurability(1);
		IncrementCombo();

		UE_LOG(LogYomiCombat, Verbose, TEXT("Weapon hit %s for %f damage (Combo: %d)"),
			*OtherActor->GetName(), DamageDealt, CurrentComboCount);
	}
}
