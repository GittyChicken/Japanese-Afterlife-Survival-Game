// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Systems/YomiToriiGate.h"
#include "Inventory/YomiInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

TArray<EResourceType> AYomiToriiGate::RestrictedResources = {
	EResourceType::IronSand,
	EResourceType::Copper,
	EResourceType::Silver,
	EResourceType::CursedIron,
	EResourceType::Obsidian,
	EResourceType::VolcanicSteel,
	EResourceType::SpiritSteel,
	EResourceType::GhostIron,
	EResourceType::SoulGem,
	EResourceType::FireCrystal
};

AYomiToriiGate::AYomiToriiGate()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	SetRootComponent(GateMesh);

	TeleportZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TeleportZone"));
	TeleportZone->SetupAttachment(GateMesh);
	TeleportZone->SetBoxExtent(FVector(100.0f, 200.0f, 300.0f));
	TeleportZone->SetCollisionProfileName(TEXT("OverlapAll"));

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(GateMesh);
	PortalEffect->bAutoActivate = false;
}

void AYomiToriiGate::BeginPlay()
{
	Super::BeginPlay();
	TeleportZone->OnComponentBeginOverlap.AddDynamic(this, &AYomiToriiGate::OnPortalOverlap);
	RegisterGate();
}

void AYomiToriiGate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterGate();
	Super::EndPlay(EndPlayReason);
}

void AYomiToriiGate::SetPortalTag(const FText& InTag)
{
	PortalTag = InTag;
	FindLinkedGate();

	if (LinkedGate)
	{
		SetPortalActive(true);
		LinkedGate->SetPortalActive(true);
	}

	UE_LOG(LogYomi, Log, TEXT("Torii Gate tag set: %s"), *PortalTag.ToString());
}

void AYomiToriiGate::FindLinkedGate()
{
	LinkedGate = nullptr;

	TArray<AActor*> AllGates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AYomiToriiGate::StaticClass(), AllGates);

	for (AActor* Actor : AllGates)
	{
		AYomiToriiGate* OtherGate = Cast<AYomiToriiGate>(Actor);
		if (OtherGate && OtherGate != this)
		{
			if (OtherGate->GetPortalTag().EqualTo(PortalTag))
			{
				LinkedGate = OtherGate;
				OtherGate->LinkedGate = this;
				UE_LOG(LogYomi, Log, TEXT("Torii Gates linked: %s"), *PortalTag.ToString());
				break;
			}
		}
	}
}

bool AYomiToriiGate::CanActorTeleport(AActor* Actor) const
{
	if (!Actor) return false;

	UYomiInventoryComponent* Inventory = Actor->FindComponentByClass<UYomiInventoryComponent>();
	if (!Inventory) return true; // No inventory = can teleport

	// Check for restricted resources (ores, metals - must be transported by boat)
	for (EResourceType RestrictedType : RestrictedResources)
	{
		FName ItemID = FName(*FString::Printf(TEXT("Resource_%d"), static_cast<uint8>(RestrictedType)));
		if (Inventory->HasItem(ItemID, 1))
		{
			UE_LOG(LogYomi, Warning, TEXT("Cannot teleport - carrying restricted resource: %d"),
				static_cast<uint8>(RestrictedType));
			return false;
		}
	}

	return true;
}

bool AYomiToriiGate::TeleportActor(AActor* ActorToTeleport)
{
	if (!IsLinked() || !bIsActive) return false;
	if (!CanActorTeleport(ActorToTeleport)) return false;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastTeleportTime < TeleportCooldown) return false;

	FVector Destination = LinkedGate->GetActorLocation() + LinkedGate->GetActorForwardVector() * 200.0f;
	FRotator DestRotation = LinkedGate->GetActorRotation();

	ActorToTeleport->SetActorLocationAndRotation(Destination, DestRotation);
	LastTeleportTime = CurrentTime;
	LinkedGate->LastTeleportTime = CurrentTime;

	UE_LOG(LogYomi, Log, TEXT("Teleported %s through Torii Gate: %s"),
		*ActorToTeleport->GetName(), *PortalTag.ToString());
	return true;
}

void AYomiToriiGate::SetPortalActive(bool bActive)
{
	bIsActive = bActive;
	if (PortalEffect)
	{
		if (bActive)
			PortalEffect->Activate(true);
		else
			PortalEffect->Deactivate();
	}
}

void AYomiToriiGate::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsLinked() && bIsActive)
	{
		TeleportActor(OtherActor);
	}
}

void AYomiToriiGate::RegisterGate()
{
	// Register with game state for tracking all gates
}

void AYomiToriiGate::UnregisterGate()
{
	if (LinkedGate)
	{
		LinkedGate->LinkedGate = nullptr;
		LinkedGate->SetPortalActive(false);
		LinkedGate = nullptr;
	}
}
