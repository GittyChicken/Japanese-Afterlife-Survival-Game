// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "World/YomiBiomeZone.h"
#include "World/YomiResourceNode.h"
#include "AI/YomiEnemyBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"

AYomiBiomeZone::AYomiBiomeZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneTrigger"));
	SetRootComponent(ZoneTrigger);
	ZoneTrigger->SetCollisionProfileName(TEXT("OverlapAll"));
	ZoneTrigger->SetGenerateOverlapEvents(true);

	AmbientEffects = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AmbientEffects"));
	AmbientEffects->SetupAttachment(ZoneTrigger);
	AmbientEffects->bAutoActivate = false;
}

void AYomiBiomeZone::BeginPlay()
{
	Super::BeginPlay();

	ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &AYomiBiomeZone::OnZoneBeginOverlap);
	ZoneTrigger->OnComponentEndOverlap.AddDynamic(this, &AYomiBiomeZone::OnZoneEndOverlap);
}

void AYomiBiomeZone::InitializeBiome(EYomiBiome InBiomeType, const FBiomeData& InBiomeData, float InRadius)
{
	BiomeType = InBiomeType;
	BiomeData = InBiomeData;
	ZoneRadius = InRadius;

	ZoneTrigger->SetSphereRadius(ZoneRadius);

	SpawnResources();
	SpawnEnemies();

	if (AmbientEffects)
	{
		AmbientEffects->Activate(true);
	}

	UE_LOG(LogYomiWorld, Log, TEXT("Biome zone initialized: %s (Difficulty: %d)"),
		*BiomeData.DisplayName.ToString(), BiomeData.DifficultyLevel);
}

bool AYomiBiomeZone::IsPointInZone(FVector Point) const
{
	return FVector::Dist(GetActorLocation(), Point) <= ZoneRadius;
}

void AYomiBiomeZone::SpawnResources()
{
	if (!GetWorld()) return;

	for (int32 i = 0; i < MaxResourceNodes; ++i)
	{
		FVector SpawnPoint = GetRandomPointInZone();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		AYomiResourceNode* Node = GetWorld()->SpawnActor<AYomiResourceNode>(
			AYomiResourceNode::StaticClass(), SpawnPoint, FRotator::ZeroRotator, SpawnParams);

		if (Node)
		{
			// Assign a random resource type from this biome's available resources
			if (BiomeData.AvailableResources.Num() > 0)
			{
				int32 RandIdx = FMath::RandRange(0, BiomeData.AvailableResources.Num() - 1);
				Node->InitializeResource(BiomeData.AvailableResources[RandIdx]);
			}
			ResourceNodes.Add(Node);
		}
	}

	UE_LOG(LogYomiWorld, Log, TEXT("Spawned %d resource nodes in %s"),
		ResourceNodes.Num(), *BiomeData.DisplayName.ToString());
}

void AYomiBiomeZone::SpawnEnemies()
{
	if (!GetWorld()) return;

	for (int32 i = 0; i < MaxEnemies; ++i)
	{
		FVector SpawnPoint = GetRandomPointInZone();

		// Select random enemy type from biome's spawnable enemies
		if (BiomeData.SpawnableEnemies.Num() > 0)
		{
			int32 RandIdx = FMath::RandRange(0, BiomeData.SpawnableEnemies.Num() - 1);
			EEnemyType EnemyType = BiomeData.SpawnableEnemies[RandIdx];

			// In full implementation, this would spawn the correct enemy subclass
			UE_LOG(LogYomiWorld, Verbose, TEXT("Spawning enemy type %d at %s"),
				static_cast<uint8>(EnemyType), *SpawnPoint.ToString());
		}
	}
}

void AYomiBiomeZone::OnPlayerEnterZone(ACharacter* Player)
{
	UE_LOG(LogYomiWorld, Log, TEXT("Player entered biome: %s"), *BiomeData.DisplayName.ToString());

	// Apply biome effects (ambient particles, fog, lighting changes)
	// This would trigger post-process volume changes, audio ambience, etc.
}

void AYomiBiomeZone::OnPlayerExitZone(ACharacter* Player)
{
	UE_LOG(LogYomiWorld, Log, TEXT("Player exited biome: %s"), *BiomeData.DisplayName.ToString());
}

void AYomiBiomeZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		OnPlayerEnterZone(Character);
	}
}

void AYomiBiomeZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		OnPlayerExitZone(Character);
	}
}

FVector AYomiBiomeZone::GetRandomPointInZone() const
{
	float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
	float Distance = FMath::FRandRange(0.0f, ZoneRadius * 0.9f);

	FVector Point = GetActorLocation();
	Point.X += FMath::Cos(Angle) * Distance;
	Point.Y += FMath::Sin(Angle) * Distance;

	return Point;
}
