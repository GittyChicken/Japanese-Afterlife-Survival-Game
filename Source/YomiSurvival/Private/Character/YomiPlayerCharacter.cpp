// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Character/YomiPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory/YomiInventoryComponent.h"
#include "Combat/YomiCombatComponent.h"
#include "Building/YomiBuildingComponent.h"
#include "AI/YomiCompanion.h"
#include "Net/UnrealNetwork.h"

AYomiPlayerCharacter::AYomiPlayerCharacter()
{
	// Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;

	// Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	BaseWalkSpeed = 600.0f;

	// Gameplay components
	InventoryComponent = CreateDefaultSubobject<UYomiInventoryComponent>(TEXT("Inventory"));
	CombatComponent = CreateDefaultSubobject<UYomiCombatComponent>(TEXT("Combat"));
	BuildingComponent = CreateDefaultSubobject<UYomiBuildingComponent>(TEXT("Building"));

	// Default stats
	MaxHealth = 100.0f;
	MaxStamina = 100.0f;
	CurrentKi = 50.0f;
	MaxKi = 50.0f;
}

void AYomiPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add input mapping context
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AYomiPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ki regeneration
	if (CurrentKi < MaxKi)
	{
		float RegenRate = KiRegenRate;
		if (bIsMeditating)
		{
			RegenRate *= MeditationKiRegenMultiplier;
		}
		RestoreKi(RegenRate * DeltaTime);
	}

	// Sprint stamina drain
	if (bIsSprinting && GetCharacterMovement()->Velocity.SizeSquared() > 0.0f)
	{
		if (!ConsumeStamina(SprintStaminaCost * DeltaTime))
		{
			StopSprint();
		}
	}

	// Dodge cooldown
	if (DodgeCooldownTimer > 0.0f)
	{
		DodgeCooldownTimer -= DeltaTime;
	}

	// Food buff tick
	TickFoodBuffs(DeltaTime);

	// Meditation healing
	if (bIsMeditating)
	{
		Heal(HealthRegenRate * 3.0f * DeltaTime);
		RestoreStamina(StaminaRegenRate * 2.0f * DeltaTime);
	}
}

void AYomiPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AYomiPlayerCharacter::Move);

		if (LookAction)
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYomiPlayerCharacter::Look);

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AYomiPlayerCharacter::StopSprint);
		}

		if (DodgeAction)
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::DodgeRoll);

		if (LightAttackAction)
			EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::LightAttack);

		if (HeavyAttackAction)
			EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::HeavyAttack);

		if (BlockAction)
		{
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::Block);
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AYomiPlayerCharacter::StopBlock);
		}

		if (SpecialAbilityAction)
			EnhancedInputComponent->BindAction(SpecialAbilityAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::UseSpecialAbility);

		if (InteractAction)
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::Interact);

		if (BuildModeAction)
			EnhancedInputComponent->BindAction(BuildModeAction, ETriggerEvent::Started, this, &AYomiPlayerCharacter::ToggleBuildMode);
	}
}

// ============================================================================
// INPUT HANDLERS
// ============================================================================

void AYomiPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsMeditating) return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AYomiPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AYomiPlayerCharacter::StartSprint()
{
	if (CurrentStamina > 0.0f && !bIsMeditating)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SprintSpeedMultiplier;
	}
}

void AYomiPlayerCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void AYomiPlayerCharacter::DodgeRoll()
{
	if (DodgeCooldownTimer > 0.0f || bIsMeditating) return;
	if (!ConsumeStamina(DodgeStaminaCost)) return;

	DodgeCooldownTimer = DodgeCooldown;

	// Launch character in movement direction
	FVector DodgeDirection = GetLastMovementInputVector();
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = GetActorForwardVector();
	}
	LaunchCharacter(DodgeDirection * 800.0f + FVector(0, 0, 100.0f), true, true);
}

void AYomiPlayerCharacter::ToggleBuildMode()
{
	if (BuildingComponent)
	{
		BuildingComponent->ToggleBuildMode();
	}
}

void AYomiPlayerCharacter::Interact()
{
	// Trace forward to find interactable
	FHitResult HitResult;
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 500.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			// Check for IInteractable interface implemented on actors
			UE_LOG(LogYomi, Verbose, TEXT("Interact trace hit: %s"), *HitActor->GetName());
		}
	}
}

void AYomiPlayerCharacter::LightAttack()
{
	if (bIsMeditating) StopMeditation();
	if (CombatComponent)
	{
		CombatComponent->ExecuteLightAttack();
	}
}

void AYomiPlayerCharacter::HeavyAttack()
{
	if (bIsMeditating) StopMeditation();
	if (CombatComponent)
	{
		CombatComponent->ExecuteHeavyAttack();
	}
}

void AYomiPlayerCharacter::Block()
{
	if (CombatComponent)
	{
		CombatComponent->StartBlocking();
	}
}

void AYomiPlayerCharacter::StopBlock()
{
	if (CombatComponent)
	{
		CombatComponent->StopBlocking();
	}
}

void AYomiPlayerCharacter::UseSpecialAbility()
{
	if (bIsMeditating) StopMeditation();
	if (CombatComponent)
	{
		CombatComponent->ExecuteSpecialAttack();
	}
}

// ============================================================================
// KI SYSTEM
// ============================================================================

bool AYomiPlayerCharacter::ConsumeKi(float Amount)
{
	if (CurrentKi < Amount) return false;

	CurrentKi = FMath::Max(0.0f, CurrentKi - Amount);
	OnKiChanged.Broadcast(CurrentKi, MaxKi);
	return true;
}

void AYomiPlayerCharacter::RestoreKi(float Amount)
{
	if (Amount <= 0.0f) return;
	CurrentKi = FMath::Clamp(CurrentKi + Amount, 0.0f, MaxKi);
	OnKiChanged.Broadcast(CurrentKi, MaxKi);
}

// ============================================================================
// HONOR SYSTEM
// ============================================================================

void AYomiPlayerCharacter::AddHonor(float Amount)
{
	HonorPoints += Amount;
	UpdateHonorLevel();
	OnHonorChanged.Broadcast(HonorPoints, CurrentHonorLevel);
}

void AYomiPlayerCharacter::RemoveHonor(float Amount)
{
	HonorPoints = FMath::Max(0.0f, HonorPoints - Amount);
	UpdateHonorLevel();
	OnHonorChanged.Broadcast(HonorPoints, CurrentHonorLevel);
}

void AYomiPlayerCharacter::OnHonorableKill(AActor* Enemy)
{
	// Bonus honor for 1v1 kills, no sneak attacks
	AddHonor(10.0f);
	UE_LOG(LogYomi, Log, TEXT("Honorable kill! +10 Honor"));
}

void AYomiPlayerCharacter::OnDishonorableAction()
{
	RemoveHonor(5.0f);
	UE_LOG(LogYomi, Log, TEXT("Dishonorable action! -5 Honor"));
}

void AYomiPlayerCharacter::UpdateHonorLevel()
{
	if (HonorPoints >= 1000.0f) CurrentHonorLevel = EHonorLevel::Ascendant;
	else if (HonorPoints >= 700.0f) CurrentHonorLevel = EHonorLevel::Legend;
	else if (HonorPoints >= 500.0f) CurrentHonorLevel = EHonorLevel::Champion;
	else if (HonorPoints >= 300.0f) CurrentHonorLevel = EHonorLevel::Samurai;
	else if (HonorPoints >= 100.0f) CurrentHonorLevel = EHonorLevel::Warrior;
	else if (HonorPoints >= 0.0f) CurrentHonorLevel = EHonorLevel::Ronin;
	else CurrentHonorLevel = EHonorLevel::Dishonored;
}

// ============================================================================
// FOOD SYSTEM
// ============================================================================

bool AYomiPlayerCharacter::ConsumeFood(const FFoodData& FoodData)
{
	if (ActiveFoodBuffs.Num() >= MaxFoodBuffSlots)
	{
		UE_LOG(LogYomi, Warning, TEXT("Cannot consume food - all %d buff slots are full"), MaxFoodBuffSlots);
		return false;
	}

	// Apply immediate effects
	Heal(FoodData.HealthRestore);
	RestoreStamina(FoodData.StaminaRestore);
	RestoreKi(FoodData.KiRestore);

	// Add buff if it has a duration
	if (FoodData.BuffDuration > 0.0f)
	{
		FActiveFoodBuff NewBuff;
		NewBuff.FoodData = FoodData;
		NewBuff.RemainingDuration = FoodData.BuffDuration;
		ActiveFoodBuffs.Add(NewBuff);
		RecalculateFoodBonuses();
	}

	UE_LOG(LogYomi, Log, TEXT("Consumed food: %s"), *FoodData.DisplayName.ToString());
	return true;
}

void AYomiPlayerCharacter::TickFoodBuffs(float DeltaTime)
{
	bool bNeedsRecalculate = false;

	for (int32 i = ActiveFoodBuffs.Num() - 1; i >= 0; --i)
	{
		ActiveFoodBuffs[i].RemainingDuration -= DeltaTime;

		// Apply per-second regen from buffs
		if (ActiveFoodBuffs[i].FoodData.HealthRegenRate > 0.0f)
		{
			Heal(ActiveFoodBuffs[i].FoodData.HealthRegenRate * DeltaTime);
		}
		if (ActiveFoodBuffs[i].FoodData.StaminaRegenRate > 0.0f)
		{
			RestoreStamina(ActiveFoodBuffs[i].FoodData.StaminaRegenRate * DeltaTime);
		}

		if (ActiveFoodBuffs[i].RemainingDuration <= 0.0f)
		{
			ActiveFoodBuffs.RemoveAt(i);
			bNeedsRecalculate = true;
		}
	}

	if (bNeedsRecalculate)
	{
		RecalculateFoodBonuses();
	}
}

void AYomiPlayerCharacter::RecalculateFoodBonuses()
{
	// Remove old bonuses
	MaxHealth -= FoodHealthBonus;
	MaxStamina -= FoodStaminaBonus;

	// Recalculate
	FoodHealthBonus = 0.0f;
	FoodStaminaBonus = 0.0f;
	FoodDamageBonus = 0.0f;
	FoodDefenseBonus = 0.0f;

	for (const FActiveFoodBuff& Buff : ActiveFoodBuffs)
	{
		FoodHealthBonus += Buff.FoodData.MaxHealthBonus;
		FoodStaminaBonus += Buff.FoodData.MaxStaminaBonus;
		FoodDamageBonus += Buff.FoodData.DamageBonus;
		FoodDefenseBonus += Buff.FoodData.DefenseBonus;
	}

	// Apply new bonuses
	MaxHealth += FoodHealthBonus;
	MaxStamina += FoodStaminaBonus;
}

// ============================================================================
// MEDITATION
// ============================================================================

void AYomiPlayerCharacter::StartMeditation()
{
	if (GetCharacterMovement()->Velocity.SizeSquared() > 1.0f) return;

	bIsMeditating = true;
	GetCharacterMovement()->DisableMovement();
	UE_LOG(LogYomi, Log, TEXT("Started meditation. Ki regen x%f"), MeditationKiRegenMultiplier);
}

void AYomiPlayerCharacter::StopMeditation()
{
	bIsMeditating = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	UE_LOG(LogYomi, Log, TEXT("Stopped meditation."));
}

// ============================================================================
// COMPANION
// ============================================================================

bool AYomiPlayerCharacter::SummonCompanion(ECompanionType CompanionType)
{
	if (ActiveCompanion)
	{
		DismissCompanion();
	}

	// Spawn companion near player
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f;
	FRotator SpawnRotation = GetActorRotation();

	// In a full implementation, this would look up the companion class from a data table
	UE_LOG(LogYomi, Log, TEXT("Summoning companion type: %d"), static_cast<uint8>(CompanionType));
	return true;
}

void AYomiPlayerCharacter::DismissCompanion()
{
	if (ActiveCompanion)
	{
		ActiveCompanion->Destroy();
		ActiveCompanion = nullptr;
		UE_LOG(LogYomi, Log, TEXT("Companion dismissed."));
	}
}

// ============================================================================
// BUSHIDO CHALLENGES
// ============================================================================

void AYomiPlayerCharacter::ActivateBushidoChallenge(EBushidoChallenge Challenge)
{
	ActiveChallenge = Challenge;
	UE_LOG(LogYomi, Log, TEXT("Bushido Challenge activated: %d"), static_cast<uint8>(Challenge));

	// Apply challenge modifiers
	switch (Challenge)
	{
	case EBushidoChallenge::NoArmor:
		// Disable armor effects - handled in combat component
		break;
	case EBushidoChallenge::KatanaOnly:
		// Restrict weapons - handled in combat component
		break;
	default:
		break;
	}
}

// ============================================================================
// PROGRESSION
// ============================================================================

void AYomiPlayerCharacter::AddCraftingExperience(float XP)
{
	CraftingXP += XP;
	while (CraftingXP >= CraftingXPToNextLevel)
	{
		CraftingXP -= CraftingXPToNextLevel;
		CraftingLevel++;
		CraftingXPToNextLevel *= 1.5f;
		UE_LOG(LogYomi, Log, TEXT("Crafting level up! Now level %d"), CraftingLevel);
	}
}

void AYomiPlayerCharacter::OnBossDefeated(EBossType Boss)
{
	if (!DefeatedBosses.Contains(Boss))
	{
		DefeatedBosses.Add(Boss);
		OnBossDefeatedEvent.Broadcast(Boss);

		// Unlock next biome
		switch (Boss)
		{
		case EBossType::KitsuneNoOkami:
			HighestUnlockedBiome = EYomiBiome::KawaNoSato;
			break;
		case EBossType::Mizuchi:
			HighestUnlockedBiome = EYomiBiome::YamaNoKage;
			break;
		case EBossType::Daitengu:
			HighestUnlockedBiome = EYomiBiome::KuroiMori;
			break;
		case EBossType::Omukade:
			HighestUnlockedBiome = EYomiBiome::KazanNoChi;
			break;
		case EBossType::ShutenDoji:
			HighestUnlockedBiome = EYomiBiome::YomiNoMon;
			break;
		case EBossType::GashadokuroTitan:
			HighestUnlockedBiome = EYomiBiome::TakamagaharaAscent;
			break;
		default:
			break;
		}

		// Award honor for boss kill
		AddHonor(50.0f);

		// Double honor if Bushido challenge active
		if (ActiveChallenge != EBushidoChallenge::None)
		{
			AddHonor(50.0f);
			UE_LOG(LogYomi, Log, TEXT("Bushido Challenge bonus honor awarded!"));
			ActiveChallenge = EBushidoChallenge::None;
		}

		UE_LOG(LogYomi, Log, TEXT("Boss defeated: %d. Unlocked biome: %d"),
			static_cast<uint8>(Boss), static_cast<uint8>(HighestUnlockedBiome));
	}
}

// ============================================================================
// TEA CEREMONY
// ============================================================================

void AYomiPlayerCharacter::StartTeaCeremony()
{
	UE_LOG(LogYomi, Log, TEXT("Tea ceremony started! Waiting for other players to join..."));
	// In multiplayer, this would broadcast to nearby players and start a group buff timer
}

void AYomiPlayerCharacter::JoinTeaCeremony(AYomiPlayerCharacter* Host)
{
	if (!Host) return;
	UE_LOG(LogYomi, Log, TEXT("Joined tea ceremony hosted by %s"), *Host->GetName());
	// Apply group buffs: health regen, stamina regen, Ki regen boost for all participants
}

// ============================================================================
// DEATH
// ============================================================================

void AYomiPlayerCharacter::Die()
{
	StopMeditation();
	StopSprint();
	DismissCompanion();

	// Remove honor on death
	RemoveHonor(10.0f);

	Super::Die();
}

void AYomiPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYomiPlayerCharacter, CurrentKi);
	DOREPLIFETIME(AYomiPlayerCharacter, MaxKi);
	DOREPLIFETIME(AYomiPlayerCharacter, HonorPoints);
	DOREPLIFETIME(AYomiPlayerCharacter, CurrentHonorLevel);
}
