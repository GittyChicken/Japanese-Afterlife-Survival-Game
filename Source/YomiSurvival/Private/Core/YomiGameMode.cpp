// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Core/YomiGameMode.h"
#include "World/YomiBiomeManager.h"
#include "Character/YomiPlayerCharacter.h"
#include "GameFramework/PlayerController.h"

AYomiGameMode::AYomiGameMode()
{
	DefaultPawnClass = AYomiPlayerCharacter::StaticClass();
}

void AYomiGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Generate world with random seed if not set
	if (WorldSeed == 0)
	{
		WorldSeed = FMath::Rand();
	}

	GenerateNewWorld(WorldSeed);
}

void AYomiGameMode::GenerateNewWorld(int32 Seed)
{
	if (Seed < 0)
	{
		Seed = FMath::Rand();
	}
	WorldSeed = Seed;

	// Spawn biome manager
	if (BiomeManagerClass)
	{
		BiomeManager = GetWorld()->SpawnActor<AYomiBiomeManager>(BiomeManagerClass);
	}
	else
	{
		BiomeManager = GetWorld()->SpawnActor<AYomiBiomeManager>();
	}

	if (BiomeManager)
	{
		BiomeManager->GenerateWorld(WorldSeed);
	}

	UE_LOG(LogYomi, Log, TEXT("World generated with seed: %d"), WorldSeed);
}

void AYomiGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Set initial spawn in Takemori (Bamboo Forest - starting area)
	PlayerRespawnPoints.Add(NewPlayer, FVector(0.0f, 0.0f, 200.0f));

	UE_LOG(LogYomi, Log, TEXT("New player joined: %s"), *NewPlayer->GetName());
}

void AYomiGameMode::RespawnPlayer(APlayerController* PC)
{
	if (!PC) return;

	FVector SpawnLocation = GetRespawnLocation(PC);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	APawn* NewPawn = SpawnDefaultPawnAtTransform(PC, FTransform(SpawnRotation, SpawnLocation));
	if (NewPawn)
	{
		PC->Possess(NewPawn);
		UE_LOG(LogYomi, Log, TEXT("Player respawned at %s"), *SpawnLocation.ToString());
	}
}

FVector AYomiGameMode::GetRespawnLocation(APlayerController* PC) const
{
	if (const FVector* Location = PlayerRespawnPoints.Find(PC))
	{
		return *Location;
	}
	return FVector(0.0f, 0.0f, 200.0f); // Default spawn
}

void AYomiGameMode::NotifyBossDefeated(EBossType Boss, AYomiPlayerCharacter* Defeater)
{
	if (!WorldDefeatedBosses.Contains(Boss))
	{
		WorldDefeatedBosses.Add(Boss);

		UE_LOG(LogYomi, Log, TEXT("Boss defeated for the first time in this world: %d"),
			static_cast<uint8>(Boss));

		// This could trigger world-wide events, unlock new areas, etc.
	}
}

bool AYomiGameMode::IsBiomeUnlocked(EYomiBiome Biome) const
{
	switch (Biome)
	{
	case EYomiBiome::Takemori:
		return true; // Always unlocked
	case EYomiBiome::KawaNoSato:
		return WorldDefeatedBosses.Contains(EBossType::KitsuneNoOkami);
	case EYomiBiome::YamaNoKage:
		return WorldDefeatedBosses.Contains(EBossType::Mizuchi);
	case EYomiBiome::KuroiMori:
		return WorldDefeatedBosses.Contains(EBossType::Daitengu);
	case EYomiBiome::KazanNoChi:
		return WorldDefeatedBosses.Contains(EBossType::Omukade);
	case EYomiBiome::YomiNoMon:
		return WorldDefeatedBosses.Contains(EBossType::ShutenDoji);
	case EYomiBiome::TakamagaharaAscent:
		return WorldDefeatedBosses.Contains(EBossType::GashadokuroTitan);
	default:
		return false;
	}
}

float AYomiGameMode::GetDifficultyMultiplier() const
{
	int32 PlayerCount = GetNumPlayers();
	float ProgressionMultiplier = 1.0f + WorldDefeatedBosses.Num() * 0.1f;
	return BaseDifficultyMultiplier + (PlayerCount - 1) * DifficultyPerPlayer * ProgressionMultiplier;
}
