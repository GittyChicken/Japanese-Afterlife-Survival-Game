// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/YomiWeaponBase.h"
#include "YomiRangedWeapon.generated.h"

class AYomiProjectile;

/**
 * Ranged weapon class for bows (Yumi/Daikyu), thrown weapons (Shuriken/Kunai),
 * blowgun (Fukiya), and matchlock gun (Teppo).
 */
UCLASS()
class YOMISURVIVAL_API AYomiRangedWeapon : public AYomiWeaponBase
{
	GENERATED_BODY()

public:
	AYomiRangedWeapon();

	virtual void StartLightAttack() override;
	virtual void StartHeavyAttack() override;
	virtual void StartSpecialAttack() override;

	/** Begin charging a shot (for bows) */
	UFUNCTION(BlueprintCallable, Category = "Ranged")
	void StartCharging();

	/** Release a charged shot */
	UFUNCTION(BlueprintCallable, Category = "Ranged")
	void ReleaseCharge();

	UFUNCTION(BlueprintPure, Category = "Ranged")
	float GetChargePercent() const;

	UFUNCTION(BlueprintPure, Category = "Ranged")
	bool IsCharging() const { return bIsCharging; }

	UFUNCTION(BlueprintPure, Category = "Ranged")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	void AddAmmo(int32 Amount);

protected:
	virtual void Tick(float DeltaTime) override;

	void FireProjectile(float ChargeMultiplier = 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<AYomiProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float ProjectileSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float MaxChargeTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float ChargeDamageMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	bool bRequiresAmmo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	int32 MaxAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	bool bCanCharge = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float ReloadTime = 1.0f;

private:
	bool bIsCharging = false;
	float CurrentChargeTime = 0.0f;
	int32 CurrentAmmo;
	float ReloadTimer = 0.0f;
};
