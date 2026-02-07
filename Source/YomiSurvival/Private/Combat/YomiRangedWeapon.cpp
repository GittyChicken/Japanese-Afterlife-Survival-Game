// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Combat/YomiRangedWeapon.h"
#include "Combat/YomiProjectile.h"

AYomiRangedWeapon::AYomiRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentAmmo = MaxAmmo;
}

void AYomiRangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCharging)
	{
		CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, MaxChargeTime);
	}

	if (ReloadTimer > 0.0f)
	{
		ReloadTimer -= DeltaTime;
	}
}

void AYomiRangedWeapon::StartLightAttack()
{
	if (bRequiresAmmo && CurrentAmmo <= 0) return;
	if (ReloadTimer > 0.0f) return;

	if (bCanCharge)
	{
		StartCharging();
	}
	else
	{
		FireProjectile(1.0f);
	}
}

void AYomiRangedWeapon::StartHeavyAttack()
{
	if (bCanCharge)
	{
		StartCharging();
	}
	else
	{
		FireProjectile(1.5f);
	}
}

void AYomiRangedWeapon::StartSpecialAttack()
{
	// Spirit arrow / special shot - uses Ki
	if (bRequiresAmmo && CurrentAmmo <= 0) return;

	FireProjectile(ChargeDamageMultiplier);
}

void AYomiRangedWeapon::StartCharging()
{
	bIsCharging = true;
	CurrentChargeTime = 0.0f;
}

void AYomiRangedWeapon::ReleaseCharge()
{
	if (!bIsCharging) return;

	float ChargePercent = GetChargePercent();
	float Multiplier = FMath::Lerp(0.5f, ChargeDamageMultiplier, ChargePercent);

	FireProjectile(Multiplier);

	bIsCharging = false;
	CurrentChargeTime = 0.0f;
}

float AYomiRangedWeapon::GetChargePercent() const
{
	if (MaxChargeTime <= 0.0f) return 1.0f;
	return FMath::Clamp(CurrentChargeTime / MaxChargeTime, 0.0f, 1.0f);
}

void AYomiRangedWeapon::AddAmmo(int32 Amount)
{
	CurrentAmmo = FMath::Min(CurrentAmmo + Amount, MaxAmmo);
}

void AYomiRangedWeapon::FireProjectile(float ChargeMultiplier)
{
	if (bRequiresAmmo)
	{
		if (CurrentAmmo <= 0) return;
		CurrentAmmo--;
	}

	if (!GetWeaponOwner()) return;

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetWeaponOwner()->GetControlRotation();

	if (ProjectileClass && GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetWeaponOwner();
		SpawnParams.Instigator = Cast<APawn>(GetWeaponOwner());

		AYomiProjectile* Projectile = GetWorld()->SpawnActor<AYomiProjectile>(
			ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (Projectile)
		{
			float Damage = CalculateDamage(false, false) * ChargeMultiplier;
			Projectile->InitializeProjectile(Damage, WeaponData.PrimaryDamageType, ProjectileSpeed * ChargeMultiplier);
		}
	}

	ReduceDurability(1);
	ReloadTimer = ReloadTime;

	UE_LOG(LogYomiCombat, Verbose, TEXT("Fired projectile with %fx charge multiplier"), ChargeMultiplier);
}
