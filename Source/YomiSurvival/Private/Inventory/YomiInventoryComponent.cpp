// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Inventory/YomiInventoryComponent.h"

UYomiInventoryComponent::UYomiInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UYomiInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize inventory slots
	InventorySlots.SetNum(InventoryCapacity);

	// Initialize hotbar
	HotbarMapping.SetNum(HotbarSize);
	for (int32 i = 0; i < HotbarSize; ++i)
	{
		HotbarMapping[i] = -1; // Unassigned
	}
}

// ============================================================================
// INVENTORY MANAGEMENT
// ============================================================================

int32 UYomiInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	if (ItemID.IsNone() || Quantity <= 0) return 0;

	int32 Remaining = Quantity;

	// First try to stack with existing items
	for (FYomiInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID && Slot.Quantity > 0)
		{
			// For now assume max stack of 99 for resources, 1 for equipment
			int32 MaxStack = 99; // Would be looked up from item data
			int32 CanAdd = MaxStack - Slot.Quantity;
			int32 ToAdd = FMath::Min(Remaining, CanAdd);

			if (ToAdd > 0)
			{
				Slot.Quantity += ToAdd;
				Remaining -= ToAdd;
			}

			if (Remaining <= 0) break;
		}
	}

	// Then fill empty slots
	while (Remaining > 0)
	{
		int32 EmptyIdx = FindEmptySlot();
		if (EmptyIdx == -1) break; // Inventory full

		int32 MaxStack = 99;
		int32 ToAdd = FMath::Min(Remaining, MaxStack);

		InventorySlots[EmptyIdx].ItemID = ItemID;
		InventorySlots[EmptyIdx].Quantity = ToAdd;
		Remaining -= ToAdd;
	}

	int32 Added = Quantity - Remaining;
	if (Added > 0)
	{
		RecalculateWeight();
		OnItemAdded.Broadcast(ItemID, Added);
		OnInventoryChanged.Broadcast();
	}

	return Added;
}

int32 UYomiInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	if (ItemID.IsNone() || Quantity <= 0) return 0;

	int32 Remaining = Quantity;

	for (FYomiInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID && Slot.Quantity > 0)
		{
			int32 ToRemove = FMath::Min(Remaining, Slot.Quantity);
			Slot.Quantity -= ToRemove;
			Remaining -= ToRemove;

			if (Slot.Quantity <= 0)
			{
				Slot.Clear();
			}

			if (Remaining <= 0) break;
		}
	}

	int32 Removed = Quantity - Remaining;
	if (Removed > 0)
	{
		RecalculateWeight();
		OnItemRemoved.Broadcast(ItemID, Removed);
		OnInventoryChanged.Broadcast();
	}

	return Removed;
}

bool UYomiInventoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
	return GetItemCount(ItemID) >= Quantity;
}

int32 UYomiInventoryComponent::GetItemCount(FName ItemID) const
{
	int32 Total = 0;
	for (const FYomiInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID)
		{
			Total += Slot.Quantity;
		}
	}
	return Total;
}

bool UYomiInventoryComponent::HasResources(const TMap<EResourceType, int32>& RequiredResources) const
{
	for (const auto& Pair : RequiredResources)
	{
		FName ItemID = ResourceTypeToItemID(Pair.Key);
		if (!HasItem(ItemID, Pair.Value))
		{
			return false;
		}
	}
	return true;
}

bool UYomiInventoryComponent::ConsumeResources(const TMap<EResourceType, int32>& RequiredResources)
{
	// First verify we have everything
	if (!HasResources(RequiredResources)) return false;

	// Then consume
	for (const auto& Pair : RequiredResources)
	{
		FName ItemID = ResourceTypeToItemID(Pair.Key);
		RemoveItem(ItemID, Pair.Value);
	}

	return true;
}

FYomiInventorySlot UYomiInventoryComponent::GetSlot(int32 Index) const
{
	if (InventorySlots.IsValidIndex(Index))
	{
		return InventorySlots[Index];
	}
	return FYomiInventorySlot();
}

void UYomiInventoryComponent::SwapSlots(int32 IndexA, int32 IndexB)
{
	if (!InventorySlots.IsValidIndex(IndexA) || !InventorySlots.IsValidIndex(IndexB)) return;
	if (IndexA == IndexB) return;

	FYomiInventorySlot Temp = InventorySlots[IndexA];
	InventorySlots[IndexA] = InventorySlots[IndexB];
	InventorySlots[IndexB] = Temp;

	OnInventoryChanged.Broadcast();
}

void UYomiInventoryComponent::DropItem(int32 SlotIndex, int32 Quantity)
{
	if (!InventorySlots.IsValidIndex(SlotIndex)) return;
	if (InventorySlots[SlotIndex].IsEmpty()) return;

	int32 ToDrop = FMath::Min(Quantity, InventorySlots[SlotIndex].Quantity);
	FName DroppedItemID = InventorySlots[SlotIndex].ItemID;

	InventorySlots[SlotIndex].Quantity -= ToDrop;
	if (InventorySlots[SlotIndex].Quantity <= 0)
	{
		InventorySlots[SlotIndex].Clear();
	}

	// Spawn world pickup (would be implemented with a pickup actor class)
	UE_LOG(LogYomiCrafting, Log, TEXT("Dropped %d x %s"), ToDrop, *DroppedItemID.ToString());

	RecalculateWeight();
	OnInventoryChanged.Broadcast();
}

// ============================================================================
// EQUIPMENT
// ============================================================================

bool UYomiInventoryComponent::EquipArmor(int32 InventorySlotIndex, EArmorSlot ArmorSlot)
{
	if (!InventorySlots.IsValidIndex(InventorySlotIndex)) return false;
	if (InventorySlots[InventorySlotIndex].IsEmpty()) return false;

	// Unequip current armor in that slot if any
	if (EquipmentSlots.Contains(ArmorSlot) && !EquipmentSlots[ArmorSlot].IsEmpty())
	{
		// Move old equipment back to inventory
		AddItem(EquipmentSlots[ArmorSlot].ItemID, 1);
	}

	// Equip new armor
	EquipmentSlots.Add(ArmorSlot, InventorySlots[InventorySlotIndex]);
	InventorySlots[InventorySlotIndex].Quantity--;
	if (InventorySlots[InventorySlotIndex].Quantity <= 0)
	{
		InventorySlots[InventorySlotIndex].Clear();
	}

	OnEquipmentChanged.Broadcast(ArmorSlot, EquipmentSlots[ArmorSlot].ItemID);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UYomiInventoryComponent::UnequipArmor(EArmorSlot ArmorSlot)
{
	if (!EquipmentSlots.Contains(ArmorSlot)) return false;
	if (EquipmentSlots[ArmorSlot].IsEmpty()) return false;

	FYomiInventorySlot ArmorItem = EquipmentSlots[ArmorSlot];
	int32 Added = AddItem(ArmorItem.ItemID, 1);

	if (Added > 0)
	{
		EquipmentSlots[ArmorSlot].Clear();
		OnEquipmentChanged.Broadcast(ArmorSlot, NAME_None);
		return true;
	}

	UE_LOG(LogYomiCrafting, Warning, TEXT("Cannot unequip - inventory is full!"));
	return false;
}

FYomiInventorySlot UYomiInventoryComponent::GetEquippedArmor(EArmorSlot Slot) const
{
	if (const FYomiInventorySlot* Found = EquipmentSlots.Find(Slot))
	{
		return *Found;
	}
	return FYomiInventorySlot();
}

float UYomiInventoryComponent::GetTotalArmorDefense() const
{
	float Total = 0.0f;
	// In full implementation, this would look up armor data from data tables
	// For each equipped armor piece, add its defense value
	return Total;
}

// ============================================================================
// HOTBAR
// ============================================================================

void UYomiInventoryComponent::SetHotbarSlot(int32 HotbarIndex, int32 InventorySlotIndex)
{
	if (!HotbarMapping.IsValidIndex(HotbarIndex)) return;
	HotbarMapping[HotbarIndex] = InventorySlotIndex;
}

int32 UYomiInventoryComponent::GetHotbarSlotInventoryIndex(int32 HotbarIndex) const
{
	if (HotbarMapping.IsValidIndex(HotbarIndex))
	{
		return HotbarMapping[HotbarIndex];
	}
	return -1;
}

void UYomiInventoryComponent::SelectHotbarSlot(int32 HotbarIndex)
{
	if (HotbarIndex >= 0 && HotbarIndex < HotbarSize)
	{
		SelectedHotbarSlot = HotbarIndex;
	}
}

// ============================================================================
// PRIVATE HELPERS
// ============================================================================

int32 UYomiInventoryComponent::FindSlotForItem(FName ItemID) const
{
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i].ItemID == ItemID && InventorySlots[i].Quantity < 99)
		{
			return i;
		}
	}
	return -1;
}

int32 UYomiInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i].IsEmpty())
		{
			return i;
		}
	}
	return -1;
}

void UYomiInventoryComponent::RecalculateWeight()
{
	CurrentWeight = 0.0f;
	for (const FYomiInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.IsEmpty())
		{
			// Weight would be looked up from item data table
			CurrentWeight += Slot.Quantity * 1.0f;
		}
	}
}

FName UYomiInventoryComponent::ResourceTypeToItemID(EResourceType ResourceType)
{
	// Maps resource enum to item ID names
	static TMap<EResourceType, FName> ResourceMap = {
		{EResourceType::Bamboo, TEXT("Resource_Bamboo")},
		{EResourceType::SakuraWood, TEXT("Resource_SakuraWood")},
		{EResourceType::CedarWood, TEXT("Resource_CedarWood")},
		{EResourceType::CryptomeriaWood, TEXT("Resource_CryptomeriaWood")},
		{EResourceType::EtherealWood, TEXT("Resource_EtherealWood")},
		{EResourceType::BasicStone, TEXT("Resource_Stone")},
		{EResourceType::IronSand, TEXT("Resource_IronSand")},
		{EResourceType::Clay, TEXT("Resource_Clay")},
		{EResourceType::Copper, TEXT("Resource_Copper")},
		{EResourceType::Silver, TEXT("Resource_Silver")},
		{EResourceType::CursedIron, TEXT("Resource_CursedIron")},
		{EResourceType::Obsidian, TEXT("Resource_Obsidian")},
		{EResourceType::VolcanicSteel, TEXT("Resource_VolcanicSteel")},
		{EResourceType::FireCrystal, TEXT("Resource_FireCrystal")},
		{EResourceType::SpiritSteel, TEXT("Resource_SpiritSteel")},
		{EResourceType::GhostIron, TEXT("Resource_GhostIron")},
		{EResourceType::Reeds, TEXT("Resource_Reeds")},
		{EResourceType::Silk, TEXT("Resource_Silk")},
		{EResourceType::Bone, TEXT("Resource_Bone")},
		{EResourceType::SacredRope, TEXT("Resource_SacredRope")},
		{EResourceType::Charcoal, TEXT("Resource_Charcoal")},
		{EResourceType::SoulGem, TEXT("Resource_SoulGem")},
		{EResourceType::WildRice, TEXT("Resource_WildRice")},
		{EResourceType::Mushroom, TEXT("Resource_Mushroom")},
		{EResourceType::Fish, TEXT("Resource_Fish")},
		{EResourceType::RareFish, TEXT("Resource_RareFish")},
		{EResourceType::MountainHerbs, TEXT("Resource_MountainHerbs")},
		{EResourceType::NightshadePlant, TEXT("Resource_Nightshade")},
		{EResourceType::Vegetables, TEXT("Resource_Vegetables")},
		{EResourceType::Chicken, TEXT("Resource_Chicken")},
		{EResourceType::Eel, TEXT("Resource_Eel")},
		{EResourceType::RiceFlour, TEXT("Resource_RiceFlour")},
		{EResourceType::GreenTea, TEXT("Resource_GreenTea")},
		{EResourceType::Sake, TEXT("Resource_Sake")},
	};

	if (const FName* Found = ResourceMap.Find(ResourceType))
	{
		return *Found;
	}
	return NAME_None;
}
