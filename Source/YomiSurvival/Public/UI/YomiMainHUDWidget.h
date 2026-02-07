// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/YomiGameTypes.h"
#include "YomiMainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UCanvasPanel;
class UHorizontalBox;

/**
 * Main HUD widget displaying health, stamina, Ki bars, honor level,
 * compass, hotbar, boss health, biome name, and notifications.
 */
UCLASS(Abstract)
class YOMISURVIVAL_API UYomiMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========================================================================
	// PLAYER BARS
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealthBar(float Current, float Max);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateStaminaBar(float Current, float Max);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateKiBar(float Current, float Max);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHonorDisplay(float HonorPoints, EHonorLevel Level);

	// ========================================================================
	// BOSS BAR
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowBossBar(FText BossName, float HealthPercent);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideBossBar();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateBossBar(float HealthPercent);

	// ========================================================================
	// NOTIFICATIONS
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DisplayNotification(FText Message, float Duration = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetInteractionPrompt(FText PromptText);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowBiomeTitle(FText BiomeName, FText JapaneseName);

	// ========================================================================
	// HOTBAR
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHotbarSlot(int32 SlotIndex, FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetSelectedHotbarSlot(int32 SlotIndex);

	// ========================================================================
	// FOOD BUFFS
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateFoodBuffDisplay(int32 SlotIndex, FText FoodName, float RemainingDuration);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Health / Stamina / Ki bars
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UProgressBar> KiBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> StaminaText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> KiText;

	// Honor
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> HonorLevelText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> HonorPointsText;

	// Boss bar
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UCanvasPanel> BossBarPanel;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UProgressBar> BossHealthBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> BossNameText;

	// Notifications
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> NotificationText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> InteractionPromptText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> BiomeNameText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> BiomeJapaneseText;

	// Hotbar
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UHorizontalBox> HotbarContainer;

private:
	float NotificationTimer = 0.0f;
	float BiomeTitleTimer = 0.0f;
};
