// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiBiomeManager.generated.h"

class AYomiBiomeZone;
class UDataTable;

/**
 * Manages the procedural world generation of Meido's fragmented islands.
 * Each island is a biome zone connected by spirit bridges or boat travel.
 * Handles biome transitions, weather, ambient effects, and resource spawning.
 */
UCLASS()
class YOMISURVIVAL_API AYomiBiomeManager : public AActor
{
	GENERATED_BODY()

public:
	AYomiBiomeManager();

	/** Generate the world with all biome islands. */
	UFUNCTION(BlueprintCallable, Category = "World")
	void GenerateWorld(int32 Seed);

	/** Get the biome at a world location. */
	UFUNCTION(BlueprintPure, Category = "World")
	EYomiBiome GetBiomeAtLocation(FVector WorldLocation) const;

	/** Get the biome data for a specific biome type. */
	UFUNCTION(BlueprintPure, Category = "World")
	FBiomeData GetBiomeData(EYomiBiome Biome) const;

	/** Get all biome zones. */
	UFUNCTION(BlueprintPure, Category = "World")
	TArray<AYomiBiomeZone*> GetAllBiomeZones() const { return BiomeZones; }

	/** Get current weather at location. */
	UFUNCTION(BlueprintPure, Category = "World|Weather")
	EWeatherType GetCurrentWeather(FVector Location) const;

	/** Trigger a weather change. */
	UFUNCTION(BlueprintCallable, Category = "World|Weather")
	void SetWeather(EYomiBiome Biome, EWeatherType Weather);

	// ========================================================================
	// DAY/NIGHT CYCLE
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "World|Time")
	float GetTimeOfDay() const { return CurrentTimeOfDay; }

	UFUNCTION(BlueprintPure, Category = "World|Time")
	bool IsNightTime() const { return CurrentTimeOfDay > 18.0f || CurrentTimeOfDay < 6.0f; }

	UFUNCTION(BlueprintCallable, Category = "World|Time")
	void SetTimeOfDay(float Hour);

	/** Speed multiplier for day/night cycle (1.0 = real-time, higher = faster) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World|Time")
	float DayNightSpeed = 50.0f;

	// ========================================================================
	// DELEGATES
	// ========================================================================

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBiomeChanged OnPlayerBiomeChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "World")
	TObjectPtr<UDataTable> BiomeDataTable;

	UPROPERTY()
	TArray<AYomiBiomeZone*> BiomeZones;

	// World generation parameters
	UPROPERTY(EditAnywhere, Category = "World|Generation")
	float IslandSpacing = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "World|Generation")
	float IslandRadius = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "World|Generation")
	int32 WorldSeed = 0;

	// Day/Night
	float CurrentTimeOfDay = 8.0f; // Start at 8 AM

	// Weather
	TMap<EYomiBiome, EWeatherType> CurrentWeatherPerBiome;
	float WeatherChangeTimer = 0.0f;
	float WeatherChangeCooldown = 300.0f; // 5 minutes between weather changes

	void TickDayNightCycle(float DeltaTime);
	void TickWeather(float DeltaTime);
	void SpawnBiomeZone(EYomiBiome BiomeType, FVector Location);
};
