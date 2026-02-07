// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "AI/YomiCompanion.h"
#include "Character/YomiPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AYomiCompanion::AYomiCompanion()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 200.0f;
	MaxStamina = 100.0f;
	HealthRegenRate = 5.0f;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	}
}

void AYomiCompanion::BeginPlay()
{
	Super::BeginPlay();
}

void AYomiCompanion::InitializeCompanion(ECompanionType InType, AYomiPlayerCharacter* InOwner)
{
	CompanionType = InType;
	CompanionOwner = InOwner;

	// Set stats based on companion type
	switch (CompanionType)
	{
	case ECompanionType::KitsuneFox:
		CompanionName = FText::FromString(TEXT("Kitsune"));
		DetectionRadius = 1200.0f; // Best at finding hidden items
		break;
	case ECompanionType::Tanuki:
		CompanionName = FText::FromString(TEXT("Tanuki"));
		DetectionRadius = 800.0f; // Carries extra items
		break;
	case ECompanionType::Kodama:
		CompanionName = FText::FromString(TEXT("Kodama"));
		DetectionRadius = 600.0f; // Boosts resource gathering
		break;
	case ECompanionType::SpiritWolf:
		CompanionName = FText::FromString(TEXT("Spirit Wolf"));
		DetectionRadius = 1000.0f; // Assists in combat
		MaxHealth = 400.0f;
		break;
	case ECompanionType::CraneSpirit:
		CompanionName = FText::FromString(TEXT("Crane Spirit"));
		DetectionRadius = 1500.0f; // Reveals map, scouts ahead
		break;
	default:
		break;
	}

	UE_LOG(LogYomiAI, Log, TEXT("Companion initialized: %s (Type: %d)"),
		*CompanionName.ToString(), static_cast<uint8>(CompanionType));
}

void AYomiCompanion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CompanionOwner)
	{
		FollowOwner(DeltaTime);

		ScanTimer += DeltaTime;
		if (ScanTimer >= ScanInterval)
		{
			ScanTimer = 0.0f;
			ScanForPointsOfInterest();
		}
	}
}

void AYomiCompanion::FollowOwner(float DeltaTime)
{
	if (!CompanionOwner) return;

	FVector OwnerLocation = CompanionOwner->GetActorLocation();
	float Distance = FVector::Dist(GetActorLocation(), OwnerLocation);

	if (Distance > FollowDistance)
	{
		FVector Direction = (OwnerLocation - GetActorLocation()).GetSafeNormal();
		AddMovementInput(Direction, 1.0f);

		// Teleport if too far
		if (Distance > FollowDistance * 10.0f)
		{
			FVector TeleportLocation = OwnerLocation - CompanionOwner->GetActorForwardVector() * FollowDistance;
			SetActorLocation(TeleportLocation);
		}
	}
}

void AYomiCompanion::ScanForPointsOfInterest()
{
	// Scan for nearby resources and enemies
	if (!GetWorld()) return;

	TArray<FHitResult> Hits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (CompanionOwner) Params.AddIgnoredActor(CompanionOwner);

	if (GetWorld()->SweepMultiByChannel(Hits, GetActorLocation(), GetActorLocation(),
		FQuat::Identity, ECC_Visibility, Sphere, Params))
	{
		for (const FHitResult& Hit : Hits)
		{
			// In full implementation, check for resource nodes and enemies
		}
	}
}

void AYomiCompanion::OnResourceDetected(AActor* Resource)
{
	UE_LOG(LogYomiAI, Log, TEXT("Companion %s detected resource: %s"),
		*CompanionName.ToString(), *Resource->GetName());
	// Play notification sound/VFX, move toward resource
}

void AYomiCompanion::OnEnemyDetected(AActor* Enemy)
{
	UE_LOG(LogYomiAI, Log, TEXT("Companion %s detected enemy: %s"),
		*CompanionName.ToString(), *Enemy->GetName());

	// Spirit Wolf will engage enemies
	if (CompanionType == ECompanionType::SpiritWolf)
	{
		// Move toward enemy and attack
	}
}

void AYomiCompanion::ActivatePassiveAbility()
{
	switch (CompanionType)
	{
	case ECompanionType::KitsuneFox:
		// Reveal hidden items and secret paths
		UE_LOG(LogYomiAI, Log, TEXT("Kitsune reveals hidden treasures nearby!"));
		break;
	case ECompanionType::Tanuki:
		// Transform into a chest, granting temporary storage
		UE_LOG(LogYomiAI, Log, TEXT("Tanuki provides extra storage!"));
		break;
	case ECompanionType::Kodama:
		// Boost resource gathering rate for nearby nodes
		UE_LOG(LogYomiAI, Log, TEXT("Kodama blesses nearby resources!"));
		break;
	case ECompanionType::SpiritWolf:
		// Howl to intimidate enemies, reducing their damage
		UE_LOG(LogYomiAI, Log, TEXT("Spirit Wolf howls! Nearby enemies weakened!"));
		break;
	case ECompanionType::CraneSpirit:
		// Fly high to reveal the surrounding area map
		UE_LOG(LogYomiAI, Log, TEXT("Crane Spirit scouts the area!"));
		break;
	default:
		break;
	}
}

void AYomiCompanion::Feed(FName FoodItemID)
{
	Loyalty = FMath::Clamp(Loyalty + 10.0f, 0.0f, MaxLoyalty);
	Heal(MaxHealth * 0.25f);
	UE_LOG(LogYomiAI, Log, TEXT("Fed companion %s. Loyalty: %f"), *CompanionName.ToString(), Loyalty);
}
