// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Systems/YomiShrineActor.h"
#include "Character/YomiPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

AYomiShrineActor::AYomiShrineActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ShrineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShrineMesh"));
	SetRootComponent(ShrineMesh);

	InteractionZone = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(ShrineMesh);
	InteractionZone->SetSphereRadius(300.0f);
	InteractionZone->SetCollisionProfileName(TEXT("OverlapAll"));

	SpiritFlameEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpiritFlame"));
	SpiritFlameEffect->SetupAttachment(ShrineMesh);
	SpiritFlameEffect->bAutoActivate = true;
}

void AYomiShrineActor::BeginPlay()
{
	Super::BeginPlay();
	InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AYomiShrineActor::OnInteractionOverlap);
}

void AYomiShrineActor::OnInteractionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(OtherActor);
	if (Player && !bDiscovered)
	{
		Discover();
	}
}

void AYomiShrineActor::Discover()
{
	bDiscovered = true;
	UE_LOG(LogYomi, Log, TEXT("Shrine discovered: %s"), *ShrineName.ToString());
	// Notify UI, play discovery VFX/sound
}

bool AYomiShrineActor::MakeOffering(FName ItemID, int32 Quantity)
{
	// Calculate offering value based on item rarity/type
	int32 Value = Quantity * 10; // Simplified
	TotalOfferingValue += Value;

	// Grant blessing based on total offerings
	if (TotalOfferingValue >= 100)
	{
		CurrentBlessing = FText::FromString(TEXT("Blessing of the Kami - +20% damage for 10 minutes"));
	}
	else if (TotalOfferingValue >= 50)
	{
		CurrentBlessing = FText::FromString(TEXT("Favor of the Kami - +10% defense for 10 minutes"));
	}
	else if (TotalOfferingValue >= 10)
	{
		CurrentBlessing = FText::FromString(TEXT("Notice of the Kami - Enhanced Ki regen for 5 minutes"));
	}

	UE_LOG(LogYomi, Log, TEXT("Offering made at %s: %s x%d (Total value: %d)"),
		*ShrineName.ToString(), *ItemID.ToString(), Quantity, TotalOfferingValue);
	return true;
}

void AYomiShrineActor::RegisterForFastTravel(FText InShrineName)
{
	ShrineName = InShrineName;
	bDiscovered = true;
	UE_LOG(LogYomi, Log, TEXT("Shrine registered for fast travel: %s"), *ShrineName.ToString());
}

void AYomiShrineActor::SetAsRespawnPoint(AActor* Player)
{
	UE_LOG(LogYomi, Log, TEXT("Shrine %s set as respawn point for %s"),
		*ShrineName.ToString(), *Player->GetName());
}
