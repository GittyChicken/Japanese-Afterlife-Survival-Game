// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

/**
 * Projectile for ranged weapons (arrows, shuriken, kunai, spirit arrows).
 */
UCLASS()
class YOMISURVIVAL_API AYomiProjectile : public AActor
{
	GENERATED_BODY()

public:
	AYomiProjectile();

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void InitializeProjectile(float InDamage, EDamageType InDamageType, float InSpeed);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	EDamageType ProjectileDamageType = EDamageType::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float LifeSpan = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bShouldStick = true;
};
