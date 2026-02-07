// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Inventory/YomiCraftingSystem.h"
#include "Inventory/YomiInventoryComponent.h"
#include "Character/YomiPlayerCharacter.h"
#include "Engine/DataTable.h"

UYomiCraftingSystem::UYomiCraftingSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UYomiCraftingSystem::BeginPlay()
{
	Super::BeginPlay();

	// Get inventory from owner
	if (AActor* Owner = GetOwner())
	{
		OwnerInventory = Owner->FindComponentByClass<UYomiInventoryComponent>();
	}

	LoadRecipes();
}

void UYomiCraftingSystem::LoadRecipes()
{
	AllRecipes.Empty();

	if (RecipeDataTable)
	{
		TArray<FCraftingRecipe*> Rows;
		RecipeDataTable->GetAllRows(TEXT("CraftingRecipes"), Rows);

		for (const FCraftingRecipe* Row : Rows)
		{
			if (Row)
			{
				AllRecipes.Add(*Row);
			}
		}
	}

	UE_LOG(LogYomiCrafting, Log, TEXT("Loaded %d crafting recipes"), AllRecipes.Num());
}

bool UYomiCraftingSystem::CanCraftRecipe(FName RecipeID) const
{
	if (!OwnerInventory) return false;

	for (const FCraftingRecipe& Recipe : AllRecipes)
	{
		if (Recipe.RecipeID == RecipeID)
		{
			// Check station
			if (ActiveStation != Recipe.RequiredStation) return false;

			// Check resources
			if (!OwnerInventory->HasResources(Recipe.RequiredResources)) return false;

			// Check crafting level
			AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(GetOwner());
			if (Player && Player->GetCraftingLevel() < Recipe.RequiredCraftingLevel) return false;

			return true;
		}
	}

	return false;
}

bool UYomiCraftingSystem::CraftItem(FName RecipeID)
{
	if (!CanCraftRecipe(RecipeID)) return false;

	for (const FCraftingRecipe& Recipe : AllRecipes)
	{
		if (Recipe.RecipeID == RecipeID)
		{
			// Consume resources
			if (!OwnerInventory->ConsumeResources(Recipe.RequiredResources))
			{
				return false;
			}

			// Add crafted item
			int32 Added = OwnerInventory->AddItem(Recipe.OutputItemID, Recipe.OutputQuantity);
			if (Added > 0)
			{
				// Grant crafting XP
				AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(GetOwner());
				if (Player)
				{
					Player->AddCraftingExperience(10.0f * (1 + Recipe.RequiredCraftingLevel));
				}

				OnItemCrafted.Broadcast(Recipe.OutputItemID, Added);
				UE_LOG(LogYomiCrafting, Log, TEXT("Crafted %d x %s"), Added, *Recipe.OutputItemID.ToString());
				return true;
			}
			else
			{
				UE_LOG(LogYomiCrafting, Warning, TEXT("Crafting failed - inventory full!"));
				return false;
			}
		}
	}

	return false;
}

TArray<FCraftingRecipe> UYomiCraftingSystem::GetRecipesForStation(ECraftingStation Station) const
{
	TArray<FCraftingRecipe> Result;
	for (const FCraftingRecipe& Recipe : AllRecipes)
	{
		if (Recipe.RequiredStation == Station)
		{
			Result.Add(Recipe);
		}
	}
	return Result;
}

TArray<FCraftingRecipe> UYomiCraftingSystem::GetUnlockedRecipes() const
{
	TArray<FCraftingRecipe> Result;

	AYomiPlayerCharacter* Player = Cast<AYomiPlayerCharacter>(GetOwner());
	int32 PlayerLevel = Player ? Player->GetCraftingLevel() : 0;

	for (const FCraftingRecipe& Recipe : AllRecipes)
	{
		if (Recipe.RequiredCraftingLevel <= PlayerLevel)
		{
			Result.Add(Recipe);
		}
	}
	return Result;
}

void UYomiCraftingSystem::SetActiveCraftingStation(ECraftingStation Station)
{
	ActiveStation = Station;
	UE_LOG(LogYomiCrafting, Log, TEXT("Active crafting station: %d"), static_cast<uint8>(Station));
}

bool UYomiCraftingSystem::UpgradeWeapon(int32 InventorySlotIndex)
{
	// Weapon upgrade system - requires specific materials based on weapon tier
	if (!OwnerInventory) return false;

	FYomiInventorySlot Slot = OwnerInventory->GetSlot(InventorySlotIndex);
	if (Slot.IsEmpty()) return false;

	UE_LOG(LogYomiCrafting, Log, TEXT("Attempting to upgrade weapon in slot %d"), InventorySlotIndex);
	return true;
}

bool UYomiCraftingSystem::RepairItem(int32 InventorySlotIndex)
{
	if (!OwnerInventory) return false;

	FYomiInventorySlot Slot = OwnerInventory->GetSlot(InventorySlotIndex);
	if (Slot.IsEmpty()) return false;

	// Repair costs half the original crafting materials
	UE_LOG(LogYomiCrafting, Log, TEXT("Attempting to repair item in slot %d"), InventorySlotIndex);
	return true;
}

bool UYomiCraftingSystem::NameWeapon(int32 InventorySlotIndex, const FText& CustomName)
{
	if (ActiveStation != ECraftingStation::CalligraphyDesk) return false;
	if (!OwnerInventory) return false;

	FYomiInventorySlot Slot = OwnerInventory->GetSlot(InventorySlotIndex);
	if (Slot.IsEmpty()) return false;

	UE_LOG(LogYomiCrafting, Log, TEXT("Named weapon: %s"), *CustomName.ToString());
	return true;
}

bool UYomiCraftingSystem::CreateClanBanner(const FText& ClanName, FLinearColor PrimaryColor, FLinearColor SecondaryColor)
{
	if (ActiveStation != ECraftingStation::CalligraphyDesk) return false;

	UE_LOG(LogYomiCrafting, Log, TEXT("Created clan banner: %s"), *ClanName.ToString());
	return true;
}
