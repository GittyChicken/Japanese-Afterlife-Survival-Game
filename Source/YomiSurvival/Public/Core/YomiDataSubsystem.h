// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/YomiGameTypes.h"
#include "YomiDataSubsystem.generated.h"

/**
 * Game instance subsystem that provides default weapon, armor, food, and biome data.
 * This serves as the master data source for all game content definitions.
 * In production, this would be driven by DataTables/DataAssets; these defaults
 * provide the complete data set defined in the game design document.
 */
UCLASS()
class YOMISURVIVAL_API UYomiDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ========================================================================
	// DATA ACCESS
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Data")
	FWeaponData GetWeaponData(EWeaponType WeaponType) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FWeaponData> GetAllWeaponsOfTier(EWeaponTier Tier) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FWeaponData> GetAllWeaponsOfClass(EWeaponClass WeaponClass) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	FArmorData GetArmorDataByID(FName ArmorID) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FArmorData> GetAllArmorOfTier(EArmorTier Tier) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	FFoodData GetFoodData(EFoodType FoodType) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FFoodData> GetAllFoods() const;

	UFUNCTION(BlueprintPure, Category = "Data")
	FBiomeData GetBiomeData(EYomiBiome Biome) const;

private:
	void InitializeWeaponData();
	void InitializeArmorData();
	void InitializeFoodData();
	void InitializeBiomeData();

	TMap<EWeaponType, FWeaponData> WeaponDatabase;
	TArray<FArmorData> ArmorDatabase;
	TMap<EFoodType, FFoodData> FoodDatabase;
	TMap<EYomiBiome, FBiomeData> BiomeDatabase;
};
