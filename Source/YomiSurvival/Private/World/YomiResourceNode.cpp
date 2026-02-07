// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "World/YomiResourceNode.h"
#include "Inventory/YomiInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AYomiResourceNode::AYomiResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	RemainingAmount = MaxAmount;
}

void AYomiResourceNode::InitializeResource(EResourceType InType, int32 InAmount)
{
	ResourceType = InType;
	if (InAmount > 0)
	{
		MaxAmount = InAmount;
	}
	RemainingAmount = MaxAmount;
}

int32 AYomiResourceNode::Harvest(AActor* Harvester, int32 Amount)
{
	if (IsDepleted() || !Harvester) return 0;

	int32 Harvested = FMath::Min(Amount, RemainingAmount);
	RemainingAmount -= Harvested;

	// Try to add to harvester's inventory
	if (UYomiInventoryComponent* Inventory = Harvester->FindComponentByClass<UYomiInventoryComponent>())
	{
		FName ItemID = FName(*FString::Printf(TEXT("Resource_%d"), static_cast<uint8>(ResourceType)));
		int32 Added = Inventory->AddItem(ItemID, Harvested);

		if (Added < Harvested)
		{
			// Put unharvested amount back
			RemainingAmount += (Harvested - Added);
			Harvested = Added;
		}
	}

	if (RemainingAmount <= 0)
	{
		OnDepleted();
	}

	UE_LOG(LogYomiWorld, Verbose, TEXT("Harvested %d of resource type %d. Remaining: %d"),
		Harvested, static_cast<uint8>(ResourceType), RemainingAmount);

	return Harvested;
}

void AYomiResourceNode::OnDepleted()
{
	MeshComponent->SetVisibility(false);
	SetActorEnableCollision(false);

	if (bCanRespawn)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AYomiResourceNode::Respawn, RespawnTime, false);
	}
}

void AYomiResourceNode::Respawn()
{
	RemainingAmount = MaxAmount;
	MeshComponent->SetVisibility(true);
	SetActorEnableCollision(true);

	UE_LOG(LogYomiWorld, Log, TEXT("Resource node respawned: type %d"), static_cast<uint8>(ResourceType));
}

void AYomiResourceNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AYomiResourceNode, RemainingAmount);
}
