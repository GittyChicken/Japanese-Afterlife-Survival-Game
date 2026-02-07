// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "World/YomiBiomeManager.h"
#include "World/YomiBiomeZone.h"
#include "Engine/DataTable.h"

AYomiBiomeManager::AYomiBiomeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AYomiBiomeManager::BeginPlay()
{
	Super::BeginPlay();

	// Initialize weather for each biome
	CurrentWeatherPerBiome.Add(EYomiBiome::Takemori, EWeatherType::Clear);
	CurrentWeatherPerBiome.Add(EYomiBiome::KawaNoSato, EWeatherType::Rain);
	CurrentWeatherPerBiome.Add(EYomiBiome::YamaNoKage, EWeatherType::Mist);
	CurrentWeatherPerBiome.Add(EYomiBiome::KuroiMori, EWeatherType::Mist);
	CurrentWeatherPerBiome.Add(EYomiBiome::KazanNoChi, EWeatherType::Clear);
	CurrentWeatherPerBiome.Add(EYomiBiome::YomiNoMon, EWeatherType::SpiritWind);
	CurrentWeatherPerBiome.Add(EYomiBiome::TakamagaharaAscent, EWeatherType::Clear);
}

void AYomiBiomeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickDayNightCycle(DeltaTime);
	TickWeather(DeltaTime);
}

void AYomiBiomeManager::GenerateWorld(int32 Seed)
{
	WorldSeed = Seed;
	FMath::RandInit(Seed);

	UE_LOG(LogYomiWorld, Log, TEXT("Generating world with seed: %d"), Seed);

	// Generate 7 biome islands in a spiral/archipelago pattern
	const TArray<EYomiBiome> BiomeOrder = {
		EYomiBiome::Takemori,
		EYomiBiome::KawaNoSato,
		EYomiBiome::YamaNoKage,
		EYomiBiome::KuroiMori,
		EYomiBiome::KazanNoChi,
		EYomiBiome::YomiNoMon,
		EYomiBiome::TakamagaharaAscent
	};

	// Arrange islands in an expanding spiral
	float AngleStep = 360.0f / BiomeOrder.Num();
	float CurrentRadius = 0.0f;

	for (int32 i = 0; i < BiomeOrder.Num(); ++i)
	{
		float Angle = FMath::DegreesToRadians(i * AngleStep + FMath::FRandRange(-15.0f, 15.0f));
		CurrentRadius = (i == 0) ? 0.0f : IslandSpacing * i;

		FVector Location(
			FMath::Cos(Angle) * CurrentRadius,
			FMath::Sin(Angle) * CurrentRadius,
			0.0f
		);

		// Add some randomization
		Location.X += FMath::FRandRange(-IslandSpacing * 0.1f, IslandSpacing * 0.1f);
		Location.Y += FMath::FRandRange(-IslandSpacing * 0.1f, IslandSpacing * 0.1f);

		SpawnBiomeZone(BiomeOrder[i], Location);
	}

	UE_LOG(LogYomiWorld, Log, TEXT("World generation complete. %d biome zones created."), BiomeZones.Num());
}

void AYomiBiomeManager::SpawnBiomeZone(EYomiBiome BiomeType, FVector Location)
{
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AYomiBiomeZone* Zone = GetWorld()->SpawnActor<AYomiBiomeZone>(
		AYomiBiomeZone::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

	if (Zone)
	{
		Zone->InitializeBiome(BiomeType, GetBiomeData(BiomeType), IslandRadius);
		BiomeZones.Add(Zone);
		UE_LOG(LogYomiWorld, Log, TEXT("Spawned biome zone: %d at %s"),
			static_cast<uint8>(BiomeType), *Location.ToString());
	}
}

EYomiBiome AYomiBiomeManager::GetBiomeAtLocation(FVector WorldLocation) const
{
	float ClosestDist = MAX_FLT;
	EYomiBiome ClosestBiome = EYomiBiome::None;

	for (const AYomiBiomeZone* Zone : BiomeZones)
	{
		if (!Zone) continue;
		float Dist = FVector::Dist(WorldLocation, Zone->GetActorLocation());
		if (Dist < ClosestDist && Dist < IslandRadius * 1.5f)
		{
			ClosestDist = Dist;
			ClosestBiome = Zone->GetBiomeType();
		}
	}

	return ClosestBiome;
}

FBiomeData AYomiBiomeManager::GetBiomeData(EYomiBiome Biome) const
{
	if (BiomeDataTable)
	{
		TArray<FBiomeData*> Rows;
		BiomeDataTable->GetAllRows(TEXT("BiomeData"), Rows);

		for (const FBiomeData* Row : Rows)
		{
			if (Row && Row->BiomeType == Biome)
			{
				return *Row;
			}
		}
	}

	// Return default data if not found in table
	FBiomeData DefaultData;
	DefaultData.BiomeType = Biome;
	DefaultData.DifficultyLevel = static_cast<int32>(Biome);
	return DefaultData;
}

EWeatherType AYomiBiomeManager::GetCurrentWeather(FVector Location) const
{
	EYomiBiome Biome = GetBiomeAtLocation(Location);
	if (const EWeatherType* Weather = CurrentWeatherPerBiome.Find(Biome))
	{
		return *Weather;
	}
	return EWeatherType::Clear;
}

void AYomiBiomeManager::SetWeather(EYomiBiome Biome, EWeatherType Weather)
{
	CurrentWeatherPerBiome.Add(Biome, Weather);
	UE_LOG(LogYomiWorld, Log, TEXT("Weather changed in biome %d to %d"),
		static_cast<uint8>(Biome), static_cast<uint8>(Weather));
}

void AYomiBiomeManager::SetTimeOfDay(float Hour)
{
	CurrentTimeOfDay = FMath::Fmod(Hour, 24.0f);
}

void AYomiBiomeManager::TickDayNightCycle(float DeltaTime)
{
	// Advance time: DayNightSpeed hours per real second
	CurrentTimeOfDay += (DeltaTime * DayNightSpeed / 3600.0f);
	if (CurrentTimeOfDay >= 24.0f)
	{
		CurrentTimeOfDay -= 24.0f;
	}
}

void AYomiBiomeManager::TickWeather(float DeltaTime)
{
	WeatherChangeTimer += DeltaTime;

	if (WeatherChangeTimer >= WeatherChangeCooldown)
	{
		WeatherChangeTimer = 0.0f;

		// Randomly change weather in one biome
		int32 BiomeIdx = FMath::RandRange(1, static_cast<int32>(EYomiBiome::TakamagaharaAscent));
		EYomiBiome Biome = static_cast<EYomiBiome>(BiomeIdx);

		int32 WeatherIdx = FMath::RandRange(0, static_cast<int32>(EWeatherType::SpiritWind));
		EWeatherType Weather = static_cast<EWeatherType>(WeatherIdx);

		SetWeather(Biome, Weather);
	}
}
