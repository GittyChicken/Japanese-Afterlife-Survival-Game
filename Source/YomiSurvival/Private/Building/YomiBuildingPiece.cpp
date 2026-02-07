// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Building/YomiBuildingPiece.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

AYomiBuildingPiece::AYomiBuildingPiece()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionBox->SetupAttachment(MeshComponent);
	CollisionBox->SetBoxExtent(FVector(100.0f));

	CurrentHealth = 100.0f;
}

void AYomiBuildingPiece::InitializePiece(const FBuildingPieceData& InData)
{
	PieceData = InData;
	CurrentHealth = PieceData.MaxHealth;

	// Load mesh if set
	if (!PieceData.Mesh.IsNull())
	{
		UStaticMesh* LoadedMesh = PieceData.Mesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshComponent->SetStaticMesh(LoadedMesh);
		}
	}

	// Set collision box to match snap size
	CollisionBox->SetBoxExtent(PieceData.SnapSize * 0.5f);

	UE_LOG(LogYomiBuilding, Log, TEXT("Building piece initialized: %s (HP: %f)"),
		*PieceData.DisplayName.ToString(), CurrentHealth);
}

float AYomiBuildingPiece::TakeDamage(float DamageAmount, EDamageType DamageType)
{
	if (bIsGhost || IsDestroyed()) return 0.0f;

	// Material resistance
	float Resistance = 0.0f;
	switch (PieceData.Material)
	{
	case EBuildingMaterial::Bamboo:
		if (DamageType == EDamageType::Fire) Resistance = -0.5f; // Extra vulnerable to fire
		break;
	case EBuildingMaterial::Stone:
		if (DamageType == EDamageType::Physical) Resistance = 0.5f;
		if (DamageType == EDamageType::Fire) Resistance = 0.8f;
		break;
	case EBuildingMaterial::Iron:
		if (DamageType == EDamageType::Physical) Resistance = 0.6f;
		break;
	case EBuildingMaterial::SpiritForged:
		Resistance = 0.3f; // Resistant to all damage types
		break;
	default:
		break;
	}

	float FinalDamage = DamageAmount * (1.0f - Resistance);
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - FinalDamage);

	if (CurrentHealth <= 0.0f)
	{
		OnPieceDestroyed();
	}

	return FinalDamage;
}

void AYomiBuildingPiece::Repair(float Amount)
{
	if (Amount < 0.0f)
	{
		// Full repair
		CurrentHealth = PieceData.MaxHealth;
	}
	else
	{
		CurrentHealth = FMath::Min(CurrentHealth + Amount, PieceData.MaxHealth);
	}
}

void AYomiBuildingPiece::SetAsGhost(bool bGhost)
{
	bIsGhost = bGhost;

	if (bGhost)
	{
		SetActorEnableCollision(false);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OriginalMaterial = MeshComponent->GetMaterial(0);
		if (GhostMaterialValid)
		{
			MeshComponent->SetMaterial(0, GhostMaterialValid);
		}
	}
	else
	{
		SetActorEnableCollision(true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		if (OriginalMaterial)
		{
			MeshComponent->SetMaterial(0, OriginalMaterial);
		}
	}
}

void AYomiBuildingPiece::SetGhostValid(bool bValid)
{
	if (!bIsGhost) return;

	if (bValid && GhostMaterialValid)
	{
		MeshComponent->SetMaterial(0, GhostMaterialValid);
	}
	else if (!bValid && GhostMaterialInvalid)
	{
		MeshComponent->SetMaterial(0, GhostMaterialInvalid);
	}
}

TArray<FVector> AYomiBuildingPiece::GetSnapPoints() const
{
	TArray<FVector> Points;
	FVector Loc = GetActorLocation();
	FVector Size = PieceData.SnapSize;

	// 4 cardinal snap points
	Points.Add(Loc + GetActorForwardVector() * Size.X);
	Points.Add(Loc - GetActorForwardVector() * Size.X);
	Points.Add(Loc + GetActorRightVector() * Size.Y);
	Points.Add(Loc - GetActorRightVector() * Size.Y);

	// Top snap point (for stacking)
	Points.Add(Loc + FVector(0, 0, Size.Z));

	return Points;
}

void AYomiBuildingPiece::OnPieceDestroyed()
{
	UE_LOG(LogYomiBuilding, Log, TEXT("Building piece destroyed: %s"), *PieceData.DisplayName.ToString());

	// Play destruction VFX
	// Drop partial materials

	SetLifeSpan(2.0f); // Destroy after animation
}

void AYomiBuildingPiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AYomiBuildingPiece, CurrentHealth);
}
