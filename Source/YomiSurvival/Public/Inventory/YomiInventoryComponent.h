// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/YomiGameTypes.h"
#include "YomiInventoryComponent.generated.h"

/**
 * A single inventory slot containing an item stack.
 */
USTRUCT(BlueprintType)
struct FYomiInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Durability = -1; // -1 means not applicable

	bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }

	void Clear()
	{
		ItemID = NAME_None;
		Quantity = 0;
		Durability = -1;
	}
};

/**
 * Inventory component handling item storage, equipment slots, hotbar,
 * resource counting, and weight management.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOMISURVIVAL_API UYomiInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UYomiInventoryComponent();

	// ========================================================================
	// INVENTORY MANAGEMENT
	// ========================================================================

	/** Add an item to the inventory. Returns how many were actually added. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(FName ItemID, int32 Quantity = 1);

	/** Remove an item from the inventory. Returns how many were actually removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(FName ItemID, int32 Quantity = 1);

	/** Check if the inventory has at least this many of an item. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemID, int32 Quantity = 1) const;

	/** Get total count of a specific item. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	/** Check if the inventory has all required resources for a recipe. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasResources(const TMap<EResourceType, int32>& RequiredResources) const;

	/** Consume resources from inventory. Returns true if successful. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeResources(const TMap<EResourceType, int32>& RequiredResources);

	/** Get the inventory slot at an index. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FYomiInventorySlot GetSlot(int32 Index) const;

	/** Swap two inventory slots. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwapSlots(int32 IndexA, int32 IndexB);

	/** Drop an item into the world. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(int32 SlotIndex, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetInventorySize() const { return InventorySlots.Num(); }

	// ========================================================================
	// EQUIPMENT
	// ========================================================================

	/** Equip an armor piece from inventory. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipArmor(int32 InventorySlotIndex, EArmorSlot ArmorSlot);

	/** Unequip an armor piece back to inventory. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipArmor(EArmorSlot ArmorSlot);

	/** Get the item in an equipment slot. */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	FYomiInventorySlot GetEquippedArmor(EArmorSlot Slot) const;

	/** Get total armor defense value. */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	float GetTotalArmorDefense() const;

	// ========================================================================
	// HOTBAR
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void SetHotbarSlot(int32 HotbarIndex, int32 InventorySlotIndex);

	UFUNCTION(BlueprintPure, Category = "Hotbar")
	int32 GetHotbarSlotInventoryIndex(int32 HotbarIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void SelectHotbarSlot(int32 HotbarIndex);

	UFUNCTION(BlueprintPure, Category = "Hotbar")
	int32 GetSelectedHotbarSlot() const { return SelectedHotbarSlot; }

	// ========================================================================
	// WEIGHT
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentWeight() const { return CurrentWeight; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetMaxWeight() const { return MaxWeight; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsOverweight() const { return CurrentWeight > MaxWeight; }

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, FName, ItemID, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, FName, ItemID, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EArmorSlot, Slot, FName, ItemID);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 InventoryCapacity = 30;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MaxWeight = 300.0f;

	float CurrentWeight = 0.0f;

	UPROPERTY()
	TArray<FYomiInventorySlot> InventorySlots;

	UPROPERTY()
	TMap<EArmorSlot, FYomiInventorySlot> EquipmentSlots;

	// Hotbar: maps hotbar index (0-9) to inventory slot index
	UPROPERTY()
	TArray<int32> HotbarMapping;

	int32 SelectedHotbarSlot = 0;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 HotbarSize = 8;

	int32 FindSlotForItem(FName ItemID) const;
	int32 FindEmptySlot() const;
	void RecalculateWeight();

	// Helper to convert resource type to item ID
	static FName ResourceTypeToItemID(EResourceType ResourceType);
};
