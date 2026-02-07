// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Core/YomiDataSubsystem.h"

void UYomiDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeWeaponData();
	InitializeArmorData();
	InitializeFoodData();
	InitializeBiomeData();

	UE_LOG(LogYomi, Log, TEXT("YomiDataSubsystem initialized: %d weapons, %d armor, %d foods, %d biomes"),
		WeaponDatabase.Num(), ArmorDatabase.Num(), FoodDatabase.Num(), BiomeDatabase.Num());
}

// ============================================================================
// WEAPON DATA - All weapons from the design document
// ============================================================================

void UYomiDataSubsystem::InitializeWeaponData()
{
	auto AddWeapon = [this](EWeaponType Type, const FString& Name, const FString& Desc,
		EWeaponTier Tier, EWeaponClass Class,
		float Damage, float Speed, float Range, float Stamina,
		float KiCost = 0.0f, EDamageType DmgType = EDamageType::Physical,
		bool CanBlock = true, float BlockReduction = 0.5f,
		bool StealthBonus = false, float StealthMult = 1.0f,
		int32 Durability = 100, float Knockback = 100.0f)
	{
		FWeaponData Data;
		Data.WeaponID = FName(*FString::Printf(TEXT("Weapon_%d"), static_cast<uint8>(Type)));
		Data.DisplayName = FText::FromString(Name);
		Data.Description = FText::FromString(Desc);
		Data.WeaponType = Type;
		Data.Tier = Tier;
		Data.WeaponClass = Class;
		Data.BaseDamage = Damage;
		Data.AttackSpeed = Speed;
		Data.Range = Range;
		Data.StaminaCost = Stamina;
		Data.KiCost = KiCost;
		Data.PrimaryDamageType = DmgType;
		Data.bCanBlock = CanBlock;
		Data.BlockDamageReduction = BlockReduction;
		Data.bHasStealthBonus = StealthBonus;
		Data.StealthDamageMultiplier = StealthMult;
		Data.MaxDurability = Durability;
		Data.KnockbackForce = Knockback;
		WeaponDatabase.Add(Type, Data);
	};

	// ========== TIER 1: BAMBOO/WOOD ==========
	AddWeapon(EWeaponType::Bokken,
		TEXT("Bokken"), TEXT("A wooden practice sword. Simple but reliable for a fallen warrior's first steps in Meido."),
		EWeaponTier::Bamboo, EWeaponClass::Sword,
		8.0f, 1.2f, 150.0f, 8.0f, 0.0f, EDamageType::Physical, true, 0.4f, false, 1.0f, 60);

	AddWeapon(EWeaponType::BambooYari,
		TEXT("Bamboo Yari"), TEXT("A bamboo spear. Provides good reach to keep yokai at a safe distance."),
		EWeaponTier::Bamboo, EWeaponClass::Spear,
		7.0f, 1.0f, 250.0f, 9.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 50);

	AddWeapon(EWeaponType::WoodenTanto,
		TEXT("Wooden Tantō"), TEXT("A wooden dagger. Fast attacks ideal for striking between enemy blows."),
		EWeaponTier::Bamboo, EWeaponClass::Dagger,
		5.0f, 2.0f, 80.0f, 5.0f, 0.0f, EDamageType::Physical, false, 0.0f, true, 1.5f, 40);

	AddWeapon(EWeaponType::BasicYumi,
		TEXT("Yumi"), TEXT("A basic bamboo bow. The weapon of choice for silent hunters in the bamboo groves."),
		EWeaponTier::Bamboo, EWeaponClass::Bow,
		10.0f, 0.8f, 2000.0f, 12.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 50);

	AddWeapon(EWeaponType::BoStaff,
		TEXT("Bō Staff"), TEXT("A bamboo staff. Defensive weapon with wide sweep attacks against multiple foes."),
		EWeaponTier::Bamboo, EWeaponClass::Staff,
		6.0f, 1.1f, 200.0f, 7.0f, 0.0f, EDamageType::Physical, true, 0.6f, false, 1.0f, 70, 150.0f);

	// ========== TIER 2: IRON ==========
	AddWeapon(EWeaponType::Wakizashi,
		TEXT("Wakizashi"), TEXT("An iron short sword. The samurai's companion blade, faster than a full katana."),
		EWeaponTier::Iron, EWeaponClass::Sword,
		15.0f, 1.5f, 130.0f, 10.0f, 0.0f, EDamageType::Physical, true, 0.45f, true, 1.3f, 120);

	AddWeapon(EWeaponType::Katana,
		TEXT("Katana"), TEXT("The soul of the samurai. A perfectly balanced blade for honorable combat."),
		EWeaponTier::Iron, EWeaponClass::Sword,
		20.0f, 1.2f, 170.0f, 12.0f, 0.0f, EDamageType::Physical, true, 0.5f, false, 1.0f, 150);

	AddWeapon(EWeaponType::Naginata,
		TEXT("Naginata"), TEXT("An iron pole blade. Sweeping strikes cleave through groups of lesser yokai."),
		EWeaponTier::Iron, EWeaponClass::Polearm,
		18.0f, 0.9f, 280.0f, 14.0f, 0.0f, EDamageType::Physical, true, 0.4f, false, 1.0f, 130);

	AddWeapon(EWeaponType::Kusarigama,
		TEXT("Kusarigama"), TEXT("A chain-sickle. Complex weapon that rewards mastery with devastating combos."),
		EWeaponTier::Iron, EWeaponClass::Chain,
		16.0f, 1.3f, 350.0f, 15.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 100);

	AddWeapon(EWeaponType::Yari,
		TEXT("Yari"), TEXT("An iron spear. The foot soldier's weapon of choice, devastating at range."),
		EWeaponTier::Iron, EWeaponClass::Spear,
		17.0f, 1.0f, 300.0f, 11.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 140);

	AddWeapon(EWeaponType::Kanabo,
		TEXT("Kanabō"), TEXT("A heavy metal club studded with iron. Shatters armor and bone alike."),
		EWeaponTier::Iron, EWeaponClass::Club,
		25.0f, 0.7f, 160.0f, 18.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 200, 250.0f);

	// ========== TIER 3: STEEL ==========
	AddWeapon(EWeaponType::Odachi,
		TEXT("Ōdachi"), TEXT("A massive two-handed great sword. Devastating but demands strength and discipline."),
		EWeaponTier::Steel, EWeaponClass::GreatSword,
		40.0f, 0.6f, 250.0f, 22.0f, 0.0f, EDamageType::Physical, true, 0.5f, false, 1.0f, 180, 300.0f);

	AddWeapon(EWeaponType::Nodachi,
		TEXT("Nodachi"), TEXT("A long field sword. Better balanced than the Ōdachi, a master swordsman's weapon."),
		EWeaponTier::Steel, EWeaponClass::GreatSword,
		35.0f, 0.8f, 230.0f, 18.0f, 0.0f, EDamageType::Physical, true, 0.5f, false, 1.0f, 170);

	AddWeapon(EWeaponType::Tetsubo,
		TEXT("Tetsubō"), TEXT("An iron-reinforced war club. The weapon of oni, now wielded by a worthy warrior."),
		EWeaponTier::Steel, EWeaponClass::Club,
		45.0f, 0.5f, 180.0f, 25.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 250, 400.0f);

	AddWeapon(EWeaponType::SteelNaginata,
		TEXT("Steel Naginata"), TEXT("A masterfully forged pole blade. Elegant and lethal in trained hands."),
		EWeaponTier::Steel, EWeaponClass::Polearm,
		30.0f, 1.0f, 300.0f, 16.0f, 0.0f, EDamageType::Physical, true, 0.45f, false, 1.0f, 160);

	AddWeapon(EWeaponType::Daikyu,
		TEXT("Daikyū"), TEXT("A great bow of steel and wood. Arrows fly true across great distances."),
		EWeaponTier::Steel, EWeaponClass::Bow,
		35.0f, 0.6f, 4000.0f, 20.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 140);

	// ========== TIER 4: SPIRIT-FORGED ==========
	AddWeapon(EWeaponType::Masamune,
		TEXT("Masamune"), TEXT("A legendary katana of perfect balance. Its blade hums with pure spirit energy."),
		EWeaponTier::Spirit, EWeaponClass::Sword,
		45.0f, 1.3f, 180.0f, 14.0f, 5.0f, EDamageType::Physical, true, 0.6f, false, 1.0f, 500);
	WeaponDatabase[EWeaponType::Masamune].SecondaryDamageType = EDamageType::Spirit;
	WeaponDatabase[EWeaponType::Masamune].SecondaryDamageAmount = 15.0f;

	AddWeapon(EWeaponType::Muramasa,
		TEXT("Muramasa"), TEXT("A cursed blade that hungers for blood. Immense power at the cost of your life force."),
		EWeaponTier::Spirit, EWeaponClass::Sword,
		60.0f, 1.4f, 180.0f, 10.0f, 0.0f, EDamageType::Physical, true, 0.4f, false, 1.0f, 300);
	WeaponDatabase[EWeaponType::Muramasa].SecondaryDamageType = EDamageType::Curse;
	WeaponDatabase[EWeaponType::Muramasa].SecondaryDamageAmount = 5.0f; // Self-damage

	AddWeapon(EWeaponType::KusanagiNoTsurugi,
		TEXT("Kusanagi-no-Tsurugi"), TEXT("The Grass-Cutting Sword of legend. Its spirit energy slashes cut through the air itself."),
		EWeaponTier::Spirit, EWeaponClass::Sword,
		50.0f, 1.1f, 400.0f, 16.0f, 15.0f, EDamageType::Spirit, true, 0.55f, false, 1.0f, 1000);
	WeaponDatabase[EWeaponType::KusanagiNoTsurugi].SecondaryDamageType = EDamageType::Wind;
	WeaponDatabase[EWeaponType::KusanagiNoTsurugi].SecondaryDamageAmount = 20.0f;

	AddWeapon(EWeaponType::Tonbogiri,
		TEXT("Tonbogiri"), TEXT("The Dragonfly Blade. So sharp a dragonfly landing on its edge was cut in two."),
		EWeaponTier::Spirit, EWeaponClass::Spear,
		55.0f, 1.0f, 350.0f, 18.0f, 10.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 800);
	WeaponDatabase[EWeaponType::Tonbogiri].SecondaryDamageType = EDamageType::Spirit;
	WeaponDatabase[EWeaponType::Tonbogiri].SecondaryDamageAmount = 10.0f;

	AddWeapon(EWeaponType::SpiritKusarigama,
		TEXT("Spirit Kusarigama"), TEXT("A chain-sickle infused with soul-binding energy. Its chains trap spirits."),
		EWeaponTier::Spirit, EWeaponClass::Chain,
		40.0f, 1.2f, 400.0f, 14.0f, 12.0f, EDamageType::Spirit, false, 0.0f, false, 1.0f, 400);

	// ========== RANGED WEAPONS ==========
	AddWeapon(EWeaponType::Shuriken,
		TEXT("Shuriken"), TEXT("Throwing stars. Quick and deadly at close range."),
		EWeaponTier::Iron, EWeaponClass::Thrown,
		8.0f, 2.5f, 1500.0f, 5.0f, 0.0f, EDamageType::Physical, false, 0.0f, true, 1.5f, 1);

	AddWeapon(EWeaponType::Kunai,
		TEXT("Kunai"), TEXT("Throwing knives. Heavier than shuriken but more damaging."),
		EWeaponTier::Iron, EWeaponClass::Thrown,
		12.0f, 1.8f, 1200.0f, 7.0f, 0.0f, EDamageType::Physical, false, 0.0f, true, 1.8f, 1);

	AddWeapon(EWeaponType::Fukiya,
		TEXT("Fukiya"), TEXT("A blowgun with poison darts. Silent and lethal from the shadows."),
		EWeaponTier::Iron, EWeaponClass::Blowgun,
		5.0f, 1.5f, 1800.0f, 4.0f, 0.0f, EDamageType::Poison, false, 0.0f, true, 2.0f, 80);

	AddWeapon(EWeaponType::Teppo,
		TEXT("Teppō"), TEXT("A matchlock gun. Rare and devastating, but slow and expensive."),
		EWeaponTier::Steel, EWeaponClass::Firearm,
		80.0f, 0.3f, 3000.0f, 5.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 100, 500.0f);

	// ========== SPECIAL / ROGUE WEAPONS ==========
	AddWeapon(EWeaponType::Ninjato,
		TEXT("Ninja-tō"), TEXT("A straight-bladed short sword favored by shinobi. Grants stealth bonuses."),
		EWeaponTier::Iron, EWeaponClass::Sword,
		14.0f, 1.6f, 120.0f, 8.0f, 0.0f, EDamageType::Physical, true, 0.35f, true, 2.0f, 110);

	AddWeapon(EWeaponType::TekkoKagi,
		TEXT("Tekko-kagi"), TEXT("Iron hand claws. Wolverine-style weapons for brutal close-quarters combat."),
		EWeaponTier::Iron, EWeaponClass::Claws,
		12.0f, 2.2f, 80.0f, 6.0f, 0.0f, EDamageType::Physical, true, 0.3f, true, 1.5f, 90);

	AddWeapon(EWeaponType::KyoketsuShoge,
		TEXT("Kyoketsu-shoge"), TEXT("A hooked rope-dart. Used for both grappling and combat at range."),
		EWeaponTier::Iron, EWeaponClass::RopeDart,
		10.0f, 1.0f, 500.0f, 10.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 100);

	AddWeapon(EWeaponType::SmokeBomb,
		TEXT("Smoke Bomb"), TEXT("Creates a cloud of smoke for escape or stealth approaches."),
		EWeaponTier::Iron, EWeaponClass::Utility,
		0.0f, 1.0f, 500.0f, 3.0f, 0.0f, EDamageType::None, false, 0.0f, false, 1.0f, 1);

	AddWeapon(EWeaponType::Caltrops,
		TEXT("Caltrops"), TEXT("Scattered metal spikes for area denial. Slows and damages pursuers."),
		EWeaponTier::Iron, EWeaponClass::Utility,
		5.0f, 1.0f, 300.0f, 3.0f, 0.0f, EDamageType::Physical, false, 0.0f, false, 1.0f, 1);
}

// ============================================================================
// ARMOR DATA
// ============================================================================

void UYomiDataSubsystem::InitializeArmorData()
{
	auto AddArmor = [this](const FString& ID, const FString& Name, const FString& Desc,
		EArmorSlot Slot, EArmorTier Tier,
		float PhysDef, float SpiritDef, float FireRes, float WaterRes, float PoisonRes,
		float MoveSpeedMod = 0.0f, float StaminaRegenMod = 0.0f,
		bool SpecialEffect = false, const FString& SpecialDesc = TEXT(""),
		int32 Durability = 100)
	{
		FArmorData Data;
		Data.ArmorID = FName(*ID);
		Data.DisplayName = FText::FromString(Name);
		Data.Description = FText::FromString(Desc);
		Data.Slot = Slot;
		Data.Tier = Tier;
		Data.PhysicalDefense = PhysDef;
		Data.SpiritDefense = SpiritDef;
		Data.FireResistance = FireRes;
		Data.WaterResistance = WaterRes;
		Data.PoisonResistance = PoisonRes;
		Data.MovementSpeedModifier = MoveSpeedMod;
		Data.StaminaRegenModifier = StaminaRegenMod;
		Data.bHasSpecialEffect = SpecialEffect;
		Data.SpecialEffectDescription = FText::FromString(SpecialDesc);
		Data.MaxDurability = Durability;
		ArmorDatabase.Add(Data);
	};

	// ===== TIER 1: PEASANT/RONIN =====
	AddArmor("Armor_StrawHat", "Sugegasa (Straw Hat)", "A simple straw hat offering minimal protection from rain.",
		EArmorSlot::Head, EArmorTier::Peasant, 2.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, false, "", 30);
	AddArmor("Armor_ClothWrap", "Cloth Wraps", "Simple cloth wraps offering meager protection.",
		EArmorSlot::Chest, EArmorTier::Peasant, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.05f, 0.05f, false, "", 30);
	AddArmor("Armor_SimpleKimono", "Simple Kimono", "A basic kimono. Light and unrestricting.",
		EArmorSlot::Chest, EArmorTier::Peasant, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, false, "", 25);
	AddArmor("Armor_LightSandals", "Light Sandals", "Simple straw sandals for traversing Meido's paths.",
		EArmorSlot::Legs, EArmorTier::Peasant, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.05f, 0.0f, false, "", 20);

	// ===== TIER 2: ASHIGARU =====
	AddArmor("Armor_LeatherDoMaru", "Leather Dō-maru", "Reinforced leather armor of a foot soldier.",
		EArmorSlot::Chest, EArmorTier::Ashigaru, 10.0f, 2.0f, 0.1f, 0.0f, 0.0f, -0.05f, 0.0f, false, "", 80);
	AddArmor("Armor_BasicKabuto", "Basic Kabuto", "A simple iron helmet for the common soldier.",
		EArmorSlot::Head, EArmorTier::Ashigaru, 8.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, "", 80);
	AddArmor("Armor_IronKote", "Iron Kote", "Iron arm guards protecting the forearms and hands.",
		EArmorSlot::Arms, EArmorTier::Ashigaru, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, "", 80);
	AddArmor("Armor_LeatherSuneate", "Leather Suneate", "Leather shin guards for the battlefield.",
		EArmorSlot::Legs, EArmorTier::Ashigaru, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.03f, 0.0f, false, "", 70);

	// ===== TIER 3: SAMURAI =====
	AddArmor("Armor_OYoroi", "Ō-yoroi Dō", "Full samurai chest plate of lamellar and plate armor.",
		EArmorSlot::Chest, EArmorTier::Samurai, 25.0f, 8.0f, 0.15f, 0.1f, 0.05f, -0.1f, -0.05f, false, "", 200);
	AddArmor("Armor_SamuraiKabuto", "Samurai Kabuto", "An ornate samurai helmet with face guard.",
		EArmorSlot::Head, EArmorTier::Samurai, 18.0f, 5.0f, 0.1f, 0.05f, 0.0f, 0.0f, 0.0f, false, "", 180);
	AddArmor("Armor_Mengu", "Mengu (Face Mask)", "A fearsome samurai face mask to intimidate foes.",
		EArmorSlot::Face, EArmorTier::Samurai, 5.0f, 3.0f, 0.05f, 0.0f, 0.1f, 0.0f, 0.0f,
		true, "Intimidation: Nearby weak enemies flee", 150);
	AddArmor("Armor_SamuraiKote", "Samurai Kote", "Heavy iron arm guards with chain mail reinforcement.",
		EArmorSlot::Arms, EArmorTier::Samurai, 15.0f, 3.0f, 0.1f, 0.0f, 0.0f, -0.03f, 0.0f, false, "", 180);
	AddArmor("Armor_SamuraiSuneate", "Samurai Suneate", "Iron shin guards with knee protection.",
		EArmorSlot::Legs, EArmorTier::Samurai, 12.0f, 2.0f, 0.05f, 0.0f, 0.0f, -0.05f, 0.0f, false, "", 180);

	// ===== TIER 4: DAIMYO/LEGENDARY =====
	AddArmor("Armor_SpiritDo", "Spirit-Forged Dō", "Armor infused with the power of conquered spirits.",
		EArmorSlot::Chest, EArmorTier::Daimyo, 40.0f, 25.0f, 0.25f, 0.2f, 0.15f, 0.0f, 0.1f,
		true, "Spirit Ward: Reduces spirit damage by 30%", 500);
	AddArmor("Armor_TenguCloak", "Tengu Wing Cloak", "A cloak woven from tengu feathers. Grants gliding ability.",
		EArmorSlot::Back, EArmorTier::Daimyo, 10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.15f, 0.1f,
		true, "Gliding: Hold jump to glide through the air", 400);
	AddArmor("Armor_OniKabuto", "Oni-Faced Kabuto", "A helmet carved in the visage of an oni.",
		EArmorSlot::Head, EArmorTier::Daimyo, 30.0f, 15.0f, 0.2f, 0.1f, 0.1f, 0.0f, 0.0f,
		true, "Terror: Enemies within range have reduced attack speed", 450);
	AddArmor("Armor_KitsuneMask", "Kitsune Mask", "A fox spirit mask granting cunning abilities.",
		EArmorSlot::Face, EArmorTier::Daimyo, 8.0f, 20.0f, 0.0f, 0.0f, 0.15f, 0.1f, 0.15f,
		true, "Illusion: Periodically creates a decoy that distracts enemies", 350);
}

// ============================================================================
// FOOD DATA
// ============================================================================

void UYomiDataSubsystem::InitializeFoodData()
{
	auto AddFood = [this](EFoodType Type, const FString& Name,
		float HealthRestore, float StaminaRestore, float KiRestore,
		float MaxHealthBonus, float MaxStaminaBonus, float Duration,
		float HealthRegen, float StaminaRegen,
		float DmgBonus, float DefBonus)
	{
		FFoodData Data;
		Data.FoodID = FName(*FString::Printf(TEXT("Food_%d"), static_cast<uint8>(Type)));
		Data.DisplayName = FText::FromString(Name);
		Data.FoodType = Type;
		Data.HealthRestore = HealthRestore;
		Data.StaminaRestore = StaminaRestore;
		Data.KiRestore = KiRestore;
		Data.MaxHealthBonus = MaxHealthBonus;
		Data.MaxStaminaBonus = MaxStaminaBonus;
		Data.BuffDuration = Duration;
		Data.HealthRegenRate = HealthRegen;
		Data.StaminaRegenRate = StaminaRegen;
		Data.DamageBonus = DmgBonus;
		Data.DefenseBonus = DefBonus;
		FoodDatabase.Add(Type, Data);
	};

	// Basic Foods
	AddFood(EFoodType::CookedRice, "Cooked Rice", 15.0f, 10.0f, 0.0f, 5.0f, 5.0f, 300.0f, 0.5f, 0.5f, 0.0f, 0.0f);
	AddFood(EFoodType::Onigiri, "Onigiri", 20.0f, 15.0f, 0.0f, 10.0f, 5.0f, 400.0f, 0.5f, 1.0f, 0.0f, 0.0f);
	AddFood(EFoodType::GrilledFish, "Grilled Fish", 25.0f, 10.0f, 5.0f, 15.0f, 0.0f, 450.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	AddFood(EFoodType::MushroomSkewers, "Mushroom Skewers", 10.0f, 20.0f, 0.0f, 0.0f, 15.0f, 350.0f, 0.0f, 1.5f, 0.0f, 0.0f);
	AddFood(EFoodType::PickledVegetables, "Pickled Vegetables", 10.0f, 5.0f, 0.0f, 5.0f, 10.0f, 600.0f, 0.3f, 0.3f, 0.0f, 0.02f);
	AddFood(EFoodType::MisoSoup, "Miso Soup", 30.0f, 5.0f, 10.0f, 10.0f, 0.0f, 300.0f, 2.0f, 0.0f, 0.0f, 0.05f);
	AddFood(EFoodType::SteamedDumplings, "Steamed Dumplings", 20.0f, 20.0f, 0.0f, 10.0f, 10.0f, 400.0f, 0.5f, 0.5f, 0.0f, 0.0f);

	// Advanced Foods
	AddFood(EFoodType::Ramen, "Ramen", 50.0f, 30.0f, 10.0f, 25.0f, 15.0f, 600.0f, 2.0f, 1.5f, 0.05f, 0.03f);
	AddFood(EFoodType::Sushi, "Sushi", 40.0f, 15.0f, 15.0f, 20.0f, 10.0f, 500.0f, 1.5f, 1.0f, 0.05f, 0.0f);
	AddFood(EFoodType::Sashimi, "Sashimi", 35.0f, 10.0f, 20.0f, 15.0f, 5.0f, 450.0f, 1.0f, 0.5f, 0.08f, 0.0f);
	AddFood(EFoodType::Tempura, "Tempura", 30.0f, 40.0f, 5.0f, 10.0f, 25.0f, 500.0f, 0.5f, 2.5f, 0.03f, 0.0f);
	AddFood(EFoodType::Yakitori, "Yakitori", 35.0f, 25.0f, 5.0f, 20.0f, 15.0f, 500.0f, 1.0f, 1.0f, 0.05f, 0.02f);
	AddFood(EFoodType::Unagi, "Unagi (Grilled Eel)", 60.0f, 40.0f, 20.0f, 30.0f, 25.0f, 900.0f, 3.0f, 2.0f, 0.1f, 0.05f);
	AddFood(EFoodType::SakeBottle, "Sake", 10.0f, 30.0f, 25.0f, -5.0f, 20.0f, 300.0f, 0.0f, 2.0f, 0.15f, -0.1f);
	AddFood(EFoodType::Dango, "Dango", 5.0f, 50.0f, 0.0f, 0.0f, 30.0f, 400.0f, 0.0f, 3.0f, 0.0f, 0.0f);
	AddFood(EFoodType::GreenTeaCup, "Green Tea", 15.0f, 10.0f, 30.0f, 5.0f, 5.0f, 600.0f, 1.0f, 0.5f, 0.0f, 0.03f);
}

// ============================================================================
// BIOME DATA
// ============================================================================

void UYomiDataSubsystem::InitializeBiomeData()
{
	auto AddBiome = [this](EYomiBiome Type, const FString& Name, const FString& Japanese,
		const FString& Desc, int32 Difficulty, EBossType Boss,
		TArray<EEnemyType> Enemies, TArray<EResourceType> Resources,
		float Temp, FLinearColor AmbientColor, FLinearColor FogCol, float Fog)
	{
		FBiomeData Data;
		Data.BiomeType = Type;
		Data.DisplayName = FText::FromString(Name);
		Data.JapaneseName = FText::FromString(Japanese);
		Data.Description = FText::FromString(Desc);
		Data.DifficultyLevel = Difficulty;
		Data.BiomeBoss = Boss;
		Data.SpawnableEnemies = Enemies;
		Data.AvailableResources = Resources;
		Data.BaseTemperature = Temp;
		Data.AmbientLightColor = AmbientColor;
		Data.FogColor = FogCol;
		Data.FogDensity = Fog;
		BiomeDatabase.Add(Type, Data);
	};

	AddBiome(EYomiBiome::Takemori, "Bamboo Forest", "竹森 (Takemori)",
		"Dense bamboo groves, cherry blossom clearings, misty valleys, and small shrines. The starting area of Meido.",
		1, EBossType::KitsuneNoOkami,
		{EEnemyType::Kitsune, EEnemyType::PossessedTanuki, EEnemyType::WildBoar, EEnemyType::BasicYokai},
		{EResourceType::Bamboo, EResourceType::SakuraWood, EResourceType::BasicStone, EResourceType::WildRice, EResourceType::Mushroom},
		22.0f, FLinearColor(0.8f, 0.9f, 0.7f, 1.0f), FLinearColor(0.6f, 0.7f, 0.6f, 1.0f), 0.02f);

	AddBiome(EYomiBiome::KawaNoSato, "River Village Ruins", "河の里 (Kawa no Sato)",
		"Abandoned fishing villages, rice paddies, rivers with torii gates, and waterfalls.",
		2, EBossType::Mizuchi,
		{EEnemyType::Kappa, EEnemyType::DrownedSpirit, EEnemyType::RiverYokai, EEnemyType::Bandit},
		{EResourceType::IronSand, EResourceType::Clay, EResourceType::Reeds, EResourceType::Fish, EResourceType::CedarWood},
		18.0f, FLinearColor(0.6f, 0.7f, 0.9f, 1.0f), FLinearColor(0.4f, 0.5f, 0.7f, 1.0f), 0.03f);

	AddBiome(EYomiBiome::YamaNoKage, "Mountain Shadow", "山の影 (Yama no Kage)",
		"Steep mountain paths, hot springs, cave shrines, mist-covered peaks, and hidden monasteries.",
		3, EBossType::Daitengu,
		{EEnemyType::Tengu, EEnemyType::MountainOni, EEnemyType::SnowYokai, EEnemyType::CorruptedMonk},
		{EResourceType::Copper, EResourceType::Silver, EResourceType::MountainHerbs, EResourceType::SacredRope, EResourceType::BasicStone},
		5.0f, FLinearColor(0.7f, 0.7f, 0.8f, 1.0f), FLinearColor(0.8f, 0.8f, 0.9f, 1.0f), 0.05f);

	AddBiome(EYomiBiome::KuroiMori, "Black Forest", "黒い森 (Kuroi Mori)",
		"Dark cryptomeria forests, abandoned castles, overgrown gardens, and spider-infested temples.",
		4, EBossType::Omukade,
		{EEnemyType::Jorogumo, EEnemyType::Gashadokuro, EEnemyType::Yurei, EEnemyType::PossessedArmor},
		{EResourceType::CryptomeriaWood, EResourceType::Silk, EResourceType::Bone, EResourceType::CursedIron, EResourceType::NightshadePlant},
		12.0f, FLinearColor(0.3f, 0.2f, 0.3f, 1.0f), FLinearColor(0.2f, 0.1f, 0.2f, 1.0f), 0.06f);

	AddBiome(EYomiBiome::KazanNoChi, "Volcanic Lands", "火山の地 (Kazan no Chi)",
		"Active volcanoes, lava flows, scorched earth, forge ruins, and fire shrines.",
		5, EBossType::ShutenDoji,
		{EEnemyType::FireDemon, EEnemyType::MagmaOni, EEnemyType::SmokeSpirit, EEnemyType::SalamanderYokai},
		{EResourceType::Obsidian, EResourceType::VolcanicSteel, EResourceType::FireCrystal, EResourceType::Charcoal},
		45.0f, FLinearColor(1.0f, 0.5f, 0.2f, 1.0f), FLinearColor(0.8f, 0.3f, 0.1f, 1.0f), 0.04f);

	AddBiome(EYomiBiome::YomiNoMon, "Gates of Yomi", "黄泉の門 (Yomi no Mon)",
		"The entrance to the underworld. Twisted landscapes, reality warping, graveyards of heroes.",
		6, EBossType::GashadokuroTitan,
		{EEnemyType::DeathSpirit, EEnemyType::SoulCollector, EEnemyType::CorruptedVariant},
		{EResourceType::SpiritSteel, EResourceType::SoulGem, EResourceType::EtherealWood, EResourceType::GhostIron},
		-5.0f, FLinearColor(0.2f, 0.1f, 0.3f, 1.0f), FLinearColor(0.15f, 0.05f, 0.2f, 1.0f), 0.08f);

	AddBiome(EYomiBiome::TakamagaharaAscent, "Takamagahara Ascent", "高天原 (Takamagahara)",
		"Floating islands, heavenly cherry blossoms, golden shrines, and spirit bridges. The final realm.",
		7, EBossType::YamataNoOrochi,
		{EEnemyType::DeathSpirit, EEnemyType::SoulCollector, EEnemyType::CorruptedVariant},
		{EResourceType::SpiritSteel, EResourceType::SoulGem, EResourceType::EtherealWood},
		20.0f, FLinearColor(1.0f, 0.95f, 0.8f, 1.0f), FLinearColor(0.9f, 0.85f, 0.7f, 1.0f), 0.01f);
}

// ============================================================================
// DATA ACCESS
// ============================================================================

FWeaponData UYomiDataSubsystem::GetWeaponData(EWeaponType WeaponType) const
{
	if (const FWeaponData* Data = WeaponDatabase.Find(WeaponType))
	{
		return *Data;
	}
	return FWeaponData();
}

TArray<FWeaponData> UYomiDataSubsystem::GetAllWeaponsOfTier(EWeaponTier Tier) const
{
	TArray<FWeaponData> Result;
	for (const auto& Pair : WeaponDatabase)
	{
		if (Pair.Value.Tier == Tier)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

TArray<FWeaponData> UYomiDataSubsystem::GetAllWeaponsOfClass(EWeaponClass WeaponClass) const
{
	TArray<FWeaponData> Result;
	for (const auto& Pair : WeaponDatabase)
	{
		if (Pair.Value.WeaponClass == WeaponClass)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

FArmorData UYomiDataSubsystem::GetArmorDataByID(FName ArmorID) const
{
	for (const FArmorData& Data : ArmorDatabase)
	{
		if (Data.ArmorID == ArmorID)
		{
			return Data;
		}
	}
	return FArmorData();
}

TArray<FArmorData> UYomiDataSubsystem::GetAllArmorOfTier(EArmorTier Tier) const
{
	TArray<FArmorData> Result;
	for (const FArmorData& Data : ArmorDatabase)
	{
		if (Data.Tier == Tier)
		{
			Result.Add(Data);
		}
	}
	return Result;
}

FFoodData UYomiDataSubsystem::GetFoodData(EFoodType FoodType) const
{
	if (const FFoodData* Data = FoodDatabase.Find(FoodType))
	{
		return *Data;
	}
	return FFoodData();
}

TArray<FFoodData> UYomiDataSubsystem::GetAllFoods() const
{
	TArray<FFoodData> Result;
	FoodDatabase.GenerateValueArray(Result);
	return Result;
}

FBiomeData UYomiDataSubsystem::GetBiomeData(EYomiBiome Biome) const
{
	if (const FBiomeData* Data = BiomeDatabase.Find(Biome))
	{
		return *Data;
	}
	return FBiomeData();
}
