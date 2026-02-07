// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/YomiGameTypes.h"
#include "YomiHUD.generated.h"

class UYomiMainHUDWidget;

/**
 * Main HUD class for Yomi Survival.
 * Manages the display of health, stamina, Ki bars, honor, compass,
 * biome indicator, boss health bars, and interaction prompts.
 */
UCLASS()
class YOMISURVIVAL_API AYomiHUD : public AHUD
{
	GENERATED_BODY()

public:
	AYomiHUD();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowBossHealthBar(FText BossName, float HealthPercent);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideBossHealthBar();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowInteractionPrompt(FText PromptText);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideInteractionPrompt();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowNotification(FText Message, float Duration = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowBiomeTransition(FText BiomeName, FText JapaneseName);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleInventoryScreen();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleCraftingScreen(ECraftingStation Station);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleMapScreen();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UYomiMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UYomiMainHUDWidget> MainHUDWidget;
};
