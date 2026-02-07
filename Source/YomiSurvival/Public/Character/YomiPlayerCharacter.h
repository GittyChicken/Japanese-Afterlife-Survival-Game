// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/YomiCharacterBase.h"
#include "InputActionValue.h"
#include "YomiPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UYomiInventoryComponent;
class UYomiCombatComponent;
class UYomiBuildingComponent;
class AYomiWeaponBase;
class AYomiCompanion;

/**
 * The player character - a fallen samurai in Meido.
 * Contains all player-specific systems: Ki, Honor, Bushido challenges,
 * food buffs, meditation, and companion management.
 */
UCLASS()
class YOMISURVIVAL_API AYomiPlayerCharacter : public AYomiCharacterBase
{
	GENERATED_BODY()

public:
	AYomiPlayerCharacter();

	// ========================================================================
	// KI (SPIRIT ENERGY) SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Ki")
	float GetCurrentKi() const { return CurrentKi; }

	UFUNCTION(BlueprintPure, Category = "Ki")
	float GetMaxKi() const { return MaxKi; }

	UFUNCTION(BlueprintPure, Category = "Ki")
	float GetKiPercent() const { return MaxKi > 0.0f ? CurrentKi / MaxKi : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Ki")
	bool ConsumeKi(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Ki")
	void RestoreKi(float Amount);

	// ========================================================================
	// HONOR SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Honor")
	float GetHonorPoints() const { return HonorPoints; }

	UFUNCTION(BlueprintPure, Category = "Honor")
	EHonorLevel GetHonorLevel() const { return CurrentHonorLevel; }

	UFUNCTION(BlueprintCallable, Category = "Honor")
	void AddHonor(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Honor")
	void RemoveHonor(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Honor")
	void OnHonorableKill(AActor* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Honor")
	void OnDishonorableAction();

	// ========================================================================
	// FOOD BUFF SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Food")
	bool ConsumeFood(const FFoodData& FoodData);

	UFUNCTION(BlueprintPure, Category = "Food")
	int32 GetActiveFoodBuffCount() const { return ActiveFoodBuffs.Num(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	int32 MaxFoodBuffSlots = 3;

	// ========================================================================
	// MEDITATION SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Meditation")
	void StartMeditation();

	UFUNCTION(BlueprintCallable, Category = "Meditation")
	void StopMeditation();

	UFUNCTION(BlueprintPure, Category = "Meditation")
	bool IsMeditating() const { return bIsMeditating; }

	// ========================================================================
	// COMPANION SYSTEM
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Companion")
	bool SummonCompanion(ECompanionType CompanionType);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void DismissCompanion();

	UFUNCTION(BlueprintPure, Category = "Companion")
	AYomiCompanion* GetActiveCompanion() const { return ActiveCompanion; }

	// ========================================================================
	// BUSHIDO CHALLENGES
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Bushido")
	void ActivateBushidoChallenge(EBushidoChallenge Challenge);

	UFUNCTION(BlueprintPure, Category = "Bushido")
	bool IsBushidoChallengeActive() const { return ActiveChallenge != EBushidoChallenge::None; }

	UFUNCTION(BlueprintPure, Category = "Bushido")
	EBushidoChallenge GetActiveChallenge() const { return ActiveChallenge; }

	// ========================================================================
	// PROGRESSION
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetCraftingLevel() const { return CraftingLevel; }

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddCraftingExperience(float XP);

	UFUNCTION(BlueprintPure, Category = "Progression")
	TArray<EBossType> GetDefeatedBosses() const { return DefeatedBosses; }

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void OnBossDefeated(EBossType Boss);

	UFUNCTION(BlueprintPure, Category = "Progression")
	EYomiBiome GetHighestUnlockedBiome() const { return HighestUnlockedBiome; }

	// ========================================================================
	// TEA CEREMONY (Multiplayer group buff)
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Social")
	void StartTeaCeremony();

	UFUNCTION(BlueprintCallable, Category = "Social")
	void JoinTeaCeremony(AYomiPlayerCharacter* Host);

	// ========================================================================
	// COMPONENTS
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "Components")
	UYomiInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintPure, Category = "Components")
	UYomiCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UFUNCTION(BlueprintPure, Category = "Components")
	UYomiBuildingComponent* GetBuildingComponent() const { return BuildingComponent; }

	// ========================================================================
	// DELEGATES
	// ========================================================================

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKiChanged OnKiChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHonorChanged OnHonorChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBossDefeated OnBossDefeatedEvent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Die() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ========================================================================
	// INPUT ACTIONS
	// ========================================================================

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void DodgeRoll();
	void ToggleBuildMode();
	void Interact();
	void LightAttack();
	void HeavyAttack();
	void Block();
	void StopBlock();
	void UseSpecialAbility();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SpecialAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> BuildModeAction;

	// ========================================================================
	// CAMERA
	// ========================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// ========================================================================
	// GAMEPLAY COMPONENTS
	// ========================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UYomiInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UYomiCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UYomiBuildingComponent> BuildingComponent;

private:
	// Ki System
	UPROPERTY(EditAnywhere, Category = "Ki", Replicated)
	float CurrentKi = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Ki", Replicated)
	float MaxKi = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Ki")
	float KiRegenRate = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Ki")
	float MeditationKiRegenMultiplier = 5.0f;

	// Honor System
	UPROPERTY(EditAnywhere, Category = "Honor", Replicated)
	float HonorPoints = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Honor", Replicated)
	EHonorLevel CurrentHonorLevel = EHonorLevel::Ronin;

	void UpdateHonorLevel();

	// Food Buffs
	struct FActiveFoodBuff
	{
		FFoodData FoodData;
		float RemainingDuration;
	};
	TArray<FActiveFoodBuff> ActiveFoodBuffs;
	void TickFoodBuffs(float DeltaTime);
	void RecalculateFoodBonuses();

	float FoodHealthBonus = 0.0f;
	float FoodStaminaBonus = 0.0f;
	float FoodDamageBonus = 0.0f;
	float FoodDefenseBonus = 0.0f;

	// Meditation
	bool bIsMeditating = false;

	// Sprint
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeedMultiplier = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintStaminaCost = 10.0f;
	float BaseWalkSpeed = 600.0f;

	// Dodge
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DodgeStaminaCost = 20.0f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DodgeCooldown = 0.5f;
	float DodgeCooldownTimer = 0.0f;

	// Companion
	UPROPERTY()
	TObjectPtr<AYomiCompanion> ActiveCompanion;

	// Bushido
	EBushidoChallenge ActiveChallenge = EBushidoChallenge::None;

	// Progression
	int32 CraftingLevel = 0;
	float CraftingXP = 0.0f;
	float CraftingXPToNextLevel = 100.0f;
	TArray<EBossType> DefeatedBosses;
	EYomiBiome HighestUnlockedBiome = EYomiBiome::Takemori;
};
