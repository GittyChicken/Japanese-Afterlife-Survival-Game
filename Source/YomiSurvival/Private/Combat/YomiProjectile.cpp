// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Combat/YomiProjectile.h"
#include "Character/YomiCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AYomiProjectile::AYomiProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	SetRootComponent(CollisionComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.3f;

	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(CollisionComponent);
	TrailEffect->bAutoActivate = true;

	InitialLifeSpan = LifeSpan;
}

void AYomiProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AYomiProjectile::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AYomiProjectile::OnOverlap);
}

void AYomiProjectile::InitializeProjectile(float InDamage, EDamageType InDamageType, float InSpeed)
{
	Damage = InDamage;
	ProjectileDamageType = InDamageType;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
		ProjectileMovement->Velocity = GetActorForwardVector() * InSpeed;
	}
}

void AYomiProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetInstigator()) return;

	AYomiCharacterBase* HitCharacter = Cast<AYomiCharacterBase>(OtherActor);
	if (HitCharacter)
	{
		HitCharacter->ApplyDamage(Damage, ProjectileDamageType, GetInstigator());
	}

	if (bShouldStick)
	{
		ProjectileMovement->StopMovementImmediately();
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(10.0f);
	}
	else
	{
		Destroy();
	}
}

void AYomiProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetInstigator()) return;

	AYomiCharacterBase* HitCharacter = Cast<AYomiCharacterBase>(OtherActor);
	if (HitCharacter)
	{
		HitCharacter->ApplyDamage(Damage, ProjectileDamageType, GetInstigator());
		Destroy();
	}
}
