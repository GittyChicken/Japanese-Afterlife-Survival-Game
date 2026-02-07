// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "YomiGameTypes.generated.h"

// ============================================================================
// LOG CATEGORIES
// ============================================================================

DECLARE_LOG_CATEGORY_EXTERN(LogYomi, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogYomiCombat, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogYomiCrafting, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogYomiWorld, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogYomiAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogYomiBuilding, Log, All);

// ============================================================================
// BIOME TYPES - The 7 Realms of Meido
// ============================================================================

UENUM(BlueprintType)
enum class EYomiBiome : uint8
{
	None					UMETA(DisplayName = "None"),
	Takemori				UMETA(DisplayName = "Takemori - Bamboo Forest"),
	KawaNoSato				UMETA(DisplayName = "Kawa no Sato - River Village Ruins"),
	YamaNoKage				UMETA(DisplayName = "Yama no Kage - Mountain Shadow"),
	KuroiMori				UMETA(DisplayName = "Kuroi Mori - Black Forest"),
	KazanNoChi				UMETA(DisplayName = "Kazan no Chi - Volcanic Lands"),
	YomiNoMon				UMETA(DisplayName = "Yomi no Mon - Gates of Yomi"),
	TakamagaharaAscent		UMETA(DisplayName = "Takamagahara Ascent - Final Realm"),

	MAX						UMETA(Hidden)
};

// ============================================================================
// WEAPON TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None					UMETA(DisplayName = "None"),

	// Tier 1 - Bamboo/Wood
	Bokken					UMETA(DisplayName = "Bokken (Wooden Sword)"),
	BambooYari				UMETA(DisplayName = "Bamboo Yari (Bamboo Spear)"),
	WoodenTanto				UMETA(DisplayName = "Wooden Tantō (Dagger)"),
	BasicYumi				UMETA(DisplayName = "Yumi (Basic Bow)"),
	BoStaff					UMETA(DisplayName = "Bō Staff"),

	// Tier 2 - Iron
	Wakizashi				UMETA(DisplayName = "Wakizashi (Short Sword)"),
	Katana					UMETA(DisplayName = "Katana"),
	Naginata				UMETA(DisplayName = "Naginata (Pole Blade)"),
	Kusarigama				UMETA(DisplayName = "Kusarigama (Chain-Sickle)"),
	Yari					UMETA(DisplayName = "Yari (Iron Spear)"),
	Kanabo					UMETA(DisplayName = "Kanabō (Metal Club)"),

	// Tier 3 - Steel
	Odachi					UMETA(DisplayName = "Ōdachi (Great Sword)"),
	Nodachi					UMETA(DisplayName = "Nodachi"),
	Tetsubo					UMETA(DisplayName = "Tetsubō (Iron Club)"),
	SteelNaginata			UMETA(DisplayName = "Steel Naginata"),
	Daikyu					UMETA(DisplayName = "Daikyū (Great Bow)"),

	// Tier 4 - Spirit-Forged
	Masamune				UMETA(DisplayName = "Masamune (Legendary Katana)"),
	Muramasa				UMETA(DisplayName = "Muramasa (Cursed Blade)"),
	KusanagiNoTsurugi		UMETA(DisplayName = "Kusanagi-no-Tsurugi"),
	Tonbogiri				UMETA(DisplayName = "Tonbogiri (Dragonfly Blade)"),
	SpiritKusarigama		UMETA(DisplayName = "Spirit Kusarigama"),

	// Ranged
	Shuriken				UMETA(DisplayName = "Shuriken"),
	Kunai					UMETA(DisplayName = "Kunai"),
	Fukiya					UMETA(DisplayName = "Fukiya (Blowgun)"),
	Teppo					UMETA(DisplayName = "Teppō (Matchlock Gun)"),

	// Special/Rogue
	Ninjato					UMETA(DisplayName = "Ninja-tō"),
	TekkoKagi				UMETA(DisplayName = "Tekko-kagi (Hand Claws)"),
	KyoketsuShoge			UMETA(DisplayName = "Kyoketsu-shoge (Rope-Dart)"),
	SmokeBomb				UMETA(DisplayName = "Smoke Bomb"),
	Caltrops				UMETA(DisplayName = "Caltrops"),

	MAX						UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponTier : uint8
{
	None		UMETA(DisplayName = "None"),
	Bamboo		UMETA(DisplayName = "Tier 1 - Bamboo/Wood"),
	Iron		UMETA(DisplayName = "Tier 2 - Iron"),
	Steel		UMETA(DisplayName = "Tier 3 - Steel"),
	Spirit		UMETA(DisplayName = "Tier 4 - Spirit-Forged"),

	MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	None			UMETA(DisplayName = "None"),
	Sword			UMETA(DisplayName = "Sword"),
	GreatSword		UMETA(DisplayName = "Great Sword"),
	Spear			UMETA(DisplayName = "Spear"),
	Polearm			UMETA(DisplayName = "Polearm"),
	Dagger			UMETA(DisplayName = "Dagger"),
	Club			UMETA(DisplayName = "Club"),
	Chain			UMETA(DisplayName = "Chain Weapon"),
	Staff			UMETA(DisplayName = "Staff"),
	Bow				UMETA(DisplayName = "Bow"),
	Thrown			UMETA(DisplayName = "Thrown"),
	Blowgun			UMETA(DisplayName = "Blowgun"),
	Firearm			UMETA(DisplayName = "Firearm"),
	Claws			UMETA(DisplayName = "Claws"),
	RopeDart		UMETA(DisplayName = "Rope-Dart"),
	Utility			UMETA(DisplayName = "Utility"),

	MAX				UMETA(Hidden)
};

// ============================================================================
// ARMOR TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	None		UMETA(DisplayName = "None"),
	Head		UMETA(DisplayName = "Head (Kabuto)"),
	Face		UMETA(DisplayName = "Face (Mengu)"),
	Chest		UMETA(DisplayName = "Chest (Dō)"),
	Arms		UMETA(DisplayName = "Arms (Kote)"),
	Legs		UMETA(DisplayName = "Legs (Suneate)"),
	Back		UMETA(DisplayName = "Back (Cape/Cloak)"),

	MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EArmorTier : uint8
{
	None		UMETA(DisplayName = "None"),
	Peasant		UMETA(DisplayName = "Tier 1 - Peasant/Ronin"),
	Ashigaru	UMETA(DisplayName = "Tier 2 - Ashigaru (Foot Soldier)"),
	Samurai		UMETA(DisplayName = "Tier 3 - Samurai"),
	Daimyo		UMETA(DisplayName = "Tier 4 - Daimyo/Legendary"),

	MAX			UMETA(Hidden)
};

// ============================================================================
// RESOURCE / ITEM TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None				UMETA(DisplayName = "None"),

	// Wood Types
	Bamboo				UMETA(DisplayName = "Bamboo"),
	SakuraWood			UMETA(DisplayName = "Sakura Wood"),
	CedarWood			UMETA(DisplayName = "Cedar Wood"),
	CryptomeriaWood		UMETA(DisplayName = "Cryptomeria (Dark Wood)"),
	EtherealWood		UMETA(DisplayName = "Ethereal Wood"),

	// Stone & Minerals
	BasicStone			UMETA(DisplayName = "Stone"),
	IronSand			UMETA(DisplayName = "Iron Sand"),
	Clay				UMETA(DisplayName = "Clay"),
	Copper				UMETA(DisplayName = "Copper"),
	Silver				UMETA(DisplayName = "Silver"),
	CursedIron			UMETA(DisplayName = "Cursed Iron"),
	Obsidian			UMETA(DisplayName = "Obsidian"),
	VolcanicSteel		UMETA(DisplayName = "Volcanic Steel Ore"),
	FireCrystal			UMETA(DisplayName = "Fire Crystal"),
	SpiritSteel			UMETA(DisplayName = "Spirit Steel"),
	GhostIron			UMETA(DisplayName = "Ghost Iron"),

	// Organic
	Reeds				UMETA(DisplayName = "Reeds"),
	Silk				UMETA(DisplayName = "Silk"),
	Bone				UMETA(DisplayName = "Bone"),
	SacredRope			UMETA(DisplayName = "Sacred Rope Materials"),
	Charcoal			UMETA(DisplayName = "Charcoal"),
	SoulGem				UMETA(DisplayName = "Soul Gem"),

	// Food Resources
	WildRice			UMETA(DisplayName = "Wild Rice"),
	Mushroom			UMETA(DisplayName = "Mushroom"),
	Fish				UMETA(DisplayName = "Fish"),
	RareFish			UMETA(DisplayName = "Rare Fish"),
	MountainHerbs		UMETA(DisplayName = "Mountain Herbs"),
	NightshadePlant		UMETA(DisplayName = "Nightshade Plant"),
	Vegetables			UMETA(DisplayName = "Vegetables"),
	Chicken				UMETA(DisplayName = "Chicken"),
	Eel					UMETA(DisplayName = "Eel"),
	RiceFlour			UMETA(DisplayName = "Rice Flour"),
	GreenTea			UMETA(DisplayName = "Green Tea Leaves"),
	Sake				UMETA(DisplayName = "Sake"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None			UMETA(DisplayName = "None"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Armor			UMETA(DisplayName = "Armor"),
	Resource		UMETA(DisplayName = "Resource"),
	Food			UMETA(DisplayName = "Food"),
	Consumable		UMETA(DisplayName = "Consumable"),
	Buildable		UMETA(DisplayName = "Buildable"),
	Quest			UMETA(DisplayName = "Quest Item"),
	Decoration		UMETA(DisplayName = "Decoration"),
	Tool			UMETA(DisplayName = "Tool"),

	MAX				UMETA(Hidden)
};

// ============================================================================
// ENEMY TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	None				UMETA(DisplayName = "None"),

	// Takemori Enemies
	Kitsune				UMETA(DisplayName = "Kitsune (Fox Spirit)"),
	PossessedTanuki		UMETA(DisplayName = "Possessed Tanuki"),
	WildBoar			UMETA(DisplayName = "Wild Boar"),
	BasicYokai			UMETA(DisplayName = "Basic Yokai"),

	// Kawa no Sato Enemies
	Kappa				UMETA(DisplayName = "Kappa (Water Demon)"),
	DrownedSpirit		UMETA(DisplayName = "Drowned Spirit"),
	RiverYokai			UMETA(DisplayName = "River Yokai"),
	Bandit				UMETA(DisplayName = "Bandit"),

	// Yama no Kage Enemies
	Tengu				UMETA(DisplayName = "Tengu (Bird Demon)"),
	MountainOni			UMETA(DisplayName = "Mountain Oni"),
	SnowYokai			UMETA(DisplayName = "Snow Yokai"),
	CorruptedMonk		UMETA(DisplayName = "Corrupted Monk"),

	// Kuroi Mori Enemies
	Jorogumo			UMETA(DisplayName = "Jorōgumo (Spider Woman)"),
	Gashadokuro			UMETA(DisplayName = "Gashadokuro (Giant Skeleton)"),
	Yurei				UMETA(DisplayName = "Yūrei (Ghost)"),
	PossessedArmor		UMETA(DisplayName = "Possessed Armor"),

	// Kazan no Chi Enemies
	FireDemon			UMETA(DisplayName = "Hi-no-Kagutsuchi Spawn"),
	MagmaOni			UMETA(DisplayName = "Magma Oni"),
	SmokeSpirit			UMETA(DisplayName = "Smoke Spirit"),
	SalamanderYokai		UMETA(DisplayName = "Salamander Yokai"),

	// Yomi no Mon Enemies
	DeathSpirit			UMETA(DisplayName = "Death Spirit"),
	SoulCollector		UMETA(DisplayName = "Soul Collector"),

	// Generic
	CorruptedVariant	UMETA(DisplayName = "Corrupted Variant"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EBossType : uint8
{
	None				UMETA(DisplayName = "None"),
	KitsuneNoOkami		UMETA(DisplayName = "Kitsune no Ōkami (Great Fox Spirit)"),
	Mizuchi				UMETA(DisplayName = "Mizuchi (Dragon-Snake)"),
	Daitengu			UMETA(DisplayName = "Daitengu (Great Tengu Lord)"),
	Omukade				UMETA(DisplayName = "Ōmukade (Giant Centipede)"),
	ShutenDoji			UMETA(DisplayName = "Shuten-dōji (Drunken Demon King)"),
	GashadokuroTitan	UMETA(DisplayName = "Gashadokuro Titan"),
	YamataNoOrochi		UMETA(DisplayName = "Yamata no Orochi (Eight-Headed Serpent)"),

	MAX					UMETA(Hidden)
};

// ============================================================================
// BUILDING TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
	None			UMETA(DisplayName = "None"),
	Foundation		UMETA(DisplayName = "Foundation"),
	Wall			UMETA(DisplayName = "Wall"),
	Floor			UMETA(DisplayName = "Floor"),
	Roof			UMETA(DisplayName = "Roof"),
	Door			UMETA(DisplayName = "Door"),
	Window			UMETA(DisplayName = "Window"),
	Stairs			UMETA(DisplayName = "Stairs"),
	Fence			UMETA(DisplayName = "Fence"),
	Defensive		UMETA(DisplayName = "Defensive"),
	CraftStation	UMETA(DisplayName = "Crafting Station"),
	Utility			UMETA(DisplayName = "Utility"),
	Decoration		UMETA(DisplayName = "Decoration"),
	Shrine			UMETA(DisplayName = "Shrine"),
	Portal			UMETA(DisplayName = "Portal (Torii Gate)"),
	Farm			UMETA(DisplayName = "Farm"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EBuildingMaterial : uint8
{
	None			UMETA(DisplayName = "None"),
	Bamboo			UMETA(DisplayName = "Bamboo"),
	Wood			UMETA(DisplayName = "Wood"),
	Stone			UMETA(DisplayName = "Stone"),
	Paper			UMETA(DisplayName = "Paper (Shoji)"),
	Tatami			UMETA(DisplayName = "Tatami"),
	Iron			UMETA(DisplayName = "Iron"),
	SpiritForged	UMETA(DisplayName = "Spirit-Forged"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECraftingStation : uint8
{
	None				UMETA(DisplayName = "None"),
	Workbench			UMETA(DisplayName = "Workbench"),
	TataraForge			UMETA(DisplayName = "Tatara Forge"),
	CarpentryStation	UMETA(DisplayName = "Carpentry Station"),
	AlchemyTable		UMETA(DisplayName = "Alchemy Table"),
	CookingStation		UMETA(DisplayName = "Cooking Station"),
	SpiritForge			UMETA(DisplayName = "Spirit Forge"),
	Dojo				UMETA(DisplayName = "Dojo"),
	CalligraphyDesk		UMETA(DisplayName = "Calligraphy Desk"),

	MAX					UMETA(Hidden)
};

// ============================================================================
// FOOD TYPES
// ============================================================================

UENUM(BlueprintType)
enum class EFoodType : uint8
{
	None				UMETA(DisplayName = "None"),

	// Basic Foods
	CookedRice			UMETA(DisplayName = "Cooked Rice"),
	Onigiri				UMETA(DisplayName = "Onigiri (Rice Ball)"),
	GrilledFish			UMETA(DisplayName = "Grilled Fish"),
	MushroomSkewers		UMETA(DisplayName = "Mushroom Skewers"),
	PickledVegetables	UMETA(DisplayName = "Pickled Vegetables"),
	MisoSoup			UMETA(DisplayName = "Miso Soup"),
	SteamedDumplings	UMETA(DisplayName = "Steamed Dumplings"),

	// Advanced Foods
	Ramen				UMETA(DisplayName = "Ramen"),
	Sushi				UMETA(DisplayName = "Sushi"),
	Sashimi				UMETA(DisplayName = "Sashimi"),
	Tempura				UMETA(DisplayName = "Tempura"),
	Yakitori			UMETA(DisplayName = "Yakitori"),
	Unagi				UMETA(DisplayName = "Unagi (Grilled Eel)"),
	SakeBottle			UMETA(DisplayName = "Sake"),
	Dango				UMETA(DisplayName = "Dango (Rice Dumplings)"),
	GreenTeaCup			UMETA(DisplayName = "Green Tea"),

	MAX					UMETA(Hidden)
};

// ============================================================================
// GAMEPLAY ENUMS
// ============================================================================

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None		UMETA(DisplayName = "None"),
	Physical	UMETA(DisplayName = "Physical"),
	Fire		UMETA(DisplayName = "Fire"),
	Water		UMETA(DisplayName = "Water"),
	Wind		UMETA(DisplayName = "Wind"),
	Earth		UMETA(DisplayName = "Earth"),
	Spirit		UMETA(DisplayName = "Spirit"),
	Poison		UMETA(DisplayName = "Poison"),
	Curse		UMETA(DisplayName = "Curse"),

	MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EHonorLevel : uint8
{
	Dishonored		UMETA(DisplayName = "Dishonored"),
	Ronin			UMETA(DisplayName = "Ronin"),
	Warrior			UMETA(DisplayName = "Warrior"),
	Samurai			UMETA(DisplayName = "Samurai"),
	Champion		UMETA(DisplayName = "Champion"),
	Legend			UMETA(DisplayName = "Legend"),
	Ascendant		UMETA(DisplayName = "Ascendant"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
	Clear		UMETA(DisplayName = "Clear"),
	Rain		UMETA(DisplayName = "Rain"),
	Snow		UMETA(DisplayName = "Snow"),
	Mist		UMETA(DisplayName = "Mist"),
	Storm		UMETA(DisplayName = "Storm"),
	SpiritWind	UMETA(DisplayName = "Spirit Wind"),

	MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECompanionType : uint8
{
	None			UMETA(DisplayName = "None"),
	KitsuneFox		UMETA(DisplayName = "Kitsune (Fox)"),
	Tanuki			UMETA(DisplayName = "Tanuki"),
	Kodama			UMETA(DisplayName = "Kodama (Tree Spirit)"),
	SpiritWolf		UMETA(DisplayName = "Spirit Wolf"),
	CraneSpirit		UMETA(DisplayName = "Crane Spirit"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EBushidoChallenge : uint8
{
	None			UMETA(DisplayName = "None"),
	NoArmor			UMETA(DisplayName = "Face boss without armor"),
	KatanaOnly		UMETA(DisplayName = "Use only a katana"),
	NoHealing		UMETA(DisplayName = "No healing items"),
	Pacifist		UMETA(DisplayName = "Kill no minions, only the boss"),
	SpeedRun		UMETA(DisplayName = "Defeat within time limit"),
	PerfectDodge	UMETA(DisplayName = "Take no damage"),

	MAX				UMETA(Hidden)
};

// ============================================================================
// DATA STRUCTURES
// ============================================================================

USTRUCT(BlueprintType)
struct FYomiItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemCategory Category = EItemCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 BaseValue = 0;
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponTier Tier = EWeaponTier::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponClass WeaponClass = EWeaponClass::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Range = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float StaminaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float KiCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EDamageType PrimaryDamageType = EDamageType::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EDamageType SecondaryDamageType = EDamageType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float SecondaryDamageAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float KnockbackForce = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanBlock = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BlockDamageReduction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bHasStealthBonus = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float StealthDamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxDurability = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> SpecialAttackMontage;

	// Crafting requirements
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Crafting")
	ECraftingStation RequiredStation = ECraftingStation::Workbench;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Crafting")
	TMap<EResourceType, int32> CraftingCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Crafting")
	int32 RequiredCraftingLevel = 0;
};

USTRUCT(BlueprintType)
struct FArmorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	FName ArmorID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	EArmorSlot Slot = EArmorSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	EArmorTier Tier = EArmorTier::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float PhysicalDefense = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float SpiritDefense = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float FireResistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float WaterResistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float PoisonResistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float MovementSpeedModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float StaminaRegenModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	bool bHasSpecialEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	FText SpecialEffectDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 MaxDurability = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor|Crafting")
	ECraftingStation RequiredStation = ECraftingStation::Workbench;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor|Crafting")
	TMap<EResourceType, int32> CraftingCost;
};

USTRUCT(BlueprintType)
struct FFoodData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	FName FoodID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	EFoodType FoodType = EFoodType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float HealthRestore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float StaminaRestore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float KiRestore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float MaxHealthBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float MaxStaminaBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float BuffDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float HealthRegenRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float StaminaRegenRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float DamageBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	float DefenseBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	TMap<EResourceType, int32> CookingIngredients;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	ECraftingStation RequiredStation = ECraftingStation::CookingStation;
};

USTRUCT(BlueprintType)
struct FCraftingRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName RecipeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName OutputItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	int32 OutputQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TMap<EResourceType, int32> RequiredResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	ECraftingStation RequiredStation = ECraftingStation::Workbench;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	int32 RequiredCraftingLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float CraftingTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	EYomiBiome RequiredBiomeProgression = EYomiBiome::None;
};

USTRUCT(BlueprintType)
struct FBiomeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	EYomiBiome BiomeType = EYomiBiome::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FText JapaneseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	int32 DifficultyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	EBossType BiomeBoss = EBossType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	TArray<EEnemyType> SpawnableEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	TArray<EResourceType> AvailableResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	TArray<EWeatherType> PossibleWeather;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	float BaseTemperature = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FLinearColor AmbientLightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FLinearColor FogColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	float FogDensity = 0.02f;
};

USTRUCT(BlueprintType)
struct FBuildingPieceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FName PieceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingCategory Category = EBuildingCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingMaterial Material = EBuildingMaterial::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TMap<EResourceType, int32> BuildCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	ECraftingStation RequiredStation = ECraftingStation::Workbench;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FVector SnapSize = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bRequiresFoundation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bProvidesRoof = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float ComfortBonus = 0.0f;
};

// Delegate declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBiomeChanged, EYomiBiome, NewBiome);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHonorChanged, float, NewHonor, EHonorLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossDefeated, EBossType, DefeatedBoss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemCrafted, FName, ItemID, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKiChanged, float, CurrentKi, float, MaxKi);
