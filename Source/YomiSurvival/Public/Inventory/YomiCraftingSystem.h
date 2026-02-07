// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/YomiGameTypes.h"
#include "YomiCraftingSystem.generated.h"

class UYomiInventoryComponent;
class UDataTable;

/**
 * Crafting system handling recipe lookup, station requirements,
 * crafting progression, and calligraphy/naming systems.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOMISURVIVAL_API UYomiCraftingSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UYomiCraftingSystem();

	/** Attempt to craft an item using a recipe. */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftItem(FName RecipeID);

	/** Check if a recipe can be crafted (has materials, correct station, correct level). */
	UFUNCTION(BlueprintPure, Category = "Crafting")
	bool CanCraftRecipe(FName RecipeID) const;

	/** Get all available recipes for a crafting station. */
	UFUNCTION(BlueprintPure, Category = "Crafting")
	TArray<FCraftingRecipe> GetRecipesForStation(ECraftingStation Station) const;

	/** Get all unlocked recipes based on progression. */
	UFUNCTION(BlueprintPure, Category = "Crafting")
	TArray<FCraftingRecipe> GetUnlockedRecipes() const;

	/** Set the current crafting station being used. */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void SetActiveCraftingStation(ECraftingStation Station);

	UFUNCTION(BlueprintPure, Category = "Crafting")
	ECraftingStation GetActiveCraftingStation() const { return ActiveStation; }

	/** Upgrade a weapon with additional materials. */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool UpgradeWeapon(int32 InventorySlotIndex);

	/** Repair a weapon or armor piece. */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool RepairItem(int32 InventorySlotIndex);

	// ========================================================================
	// CALLIGRAPHY SYSTEM
	// ========================================================================

	/** Name a weapon (calligraphy desk required). */
	UFUNCTION(BlueprintCallable, Category = "Crafting|Calligraphy")
	bool NameWeapon(int32 InventorySlotIndex, const FText& CustomName);

	/** Create a clan banner with custom symbol. */
	UFUNCTION(BlueprintCallable, Category = "Crafting|Calligraphy")
	bool CreateClanBanner(const FText& ClanName, FLinearColor PrimaryColor, FLinearColor SecondaryColor);

	// ========================================================================
	// DELEGATES
	// ========================================================================

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemCrafted OnItemCrafted;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UYomiInventoryComponent> OwnerInventory;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	TObjectPtr<UDataTable> RecipeDataTable;

	ECraftingStation ActiveStation = ECraftingStation::None;

	// Cache of all recipes
	TArray<FCraftingRecipe> AllRecipes;

	void LoadRecipes();
};
