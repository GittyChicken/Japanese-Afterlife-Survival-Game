// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "UI/YomiMainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"

void UYomiMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BossBarPanel) BossBarPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (NotificationText) NotificationText->SetVisibility(ESlateVisibility::Collapsed);
	if (BiomeNameText) BiomeNameText->SetVisibility(ESlateVisibility::Collapsed);
	if (BiomeJapaneseText) BiomeJapaneseText->SetVisibility(ESlateVisibility::Collapsed);
}

void UYomiMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Notification fade
	if (NotificationTimer > 0.0f)
	{
		NotificationTimer -= InDeltaTime;
		if (NotificationTimer <= 0.0f && NotificationText)
		{
			NotificationText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Biome title fade
	if (BiomeTitleTimer > 0.0f)
	{
		BiomeTitleTimer -= InDeltaTime;
		if (BiomeTitleTimer <= 0.0f)
		{
			if (BiomeNameText) BiomeNameText->SetVisibility(ESlateVisibility::Collapsed);
			if (BiomeJapaneseText) BiomeJapaneseText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UYomiMainHUDWidget::UpdateHealthBar(float Current, float Max)
{
	if (HealthBar) HealthBar->SetPercent(Max > 0.0f ? Current / Max : 0.0f);
	if (HealthText) HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Current, Max)));
}

void UYomiMainHUDWidget::UpdateStaminaBar(float Current, float Max)
{
	if (StaminaBar) StaminaBar->SetPercent(Max > 0.0f ? Current / Max : 0.0f);
	if (StaminaText) StaminaText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Current, Max)));
}

void UYomiMainHUDWidget::UpdateKiBar(float Current, float Max)
{
	if (KiBar) KiBar->SetPercent(Max > 0.0f ? Current / Max : 0.0f);
	if (KiText) KiText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Current, Max)));
}

void UYomiMainHUDWidget::UpdateHonorDisplay(float HonorPoints, EHonorLevel Level)
{
	if (HonorPointsText)
	{
		HonorPointsText->SetText(FText::FromString(FString::Printf(TEXT("Honor: %.0f"), HonorPoints)));
	}

	if (HonorLevelText)
	{
		FString LevelName;
		switch (Level)
		{
		case EHonorLevel::Dishonored: LevelName = TEXT("Dishonored"); break;
		case EHonorLevel::Ronin: LevelName = TEXT("Ronin"); break;
		case EHonorLevel::Warrior: LevelName = TEXT("Warrior"); break;
		case EHonorLevel::Samurai: LevelName = TEXT("Samurai"); break;
		case EHonorLevel::Champion: LevelName = TEXT("Champion"); break;
		case EHonorLevel::Legend: LevelName = TEXT("Legend"); break;
		case EHonorLevel::Ascendant: LevelName = TEXT("Ascendant"); break;
		default: LevelName = TEXT("Unknown"); break;
		}
		HonorLevelText->SetText(FText::FromString(LevelName));
	}
}

void UYomiMainHUDWidget::ShowBossBar(FText BossName, float HealthPercent)
{
	if (BossBarPanel) BossBarPanel->SetVisibility(ESlateVisibility::Visible);
	if (BossNameText) BossNameText->SetText(BossName);
	UpdateBossBar(HealthPercent);
}

void UYomiMainHUDWidget::HideBossBar()
{
	if (BossBarPanel) BossBarPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UYomiMainHUDWidget::UpdateBossBar(float HealthPercent)
{
	if (BossHealthBar) BossHealthBar->SetPercent(HealthPercent);
}

void UYomiMainHUDWidget::DisplayNotification(FText Message, float Duration)
{
	if (NotificationText)
	{
		NotificationText->SetText(Message);
		NotificationText->SetVisibility(ESlateVisibility::Visible);
		NotificationTimer = Duration;
	}
}

void UYomiMainHUDWidget::SetInteractionPrompt(FText PromptText)
{
	if (InteractionPromptText)
	{
		if (PromptText.IsEmpty())
		{
			InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			InteractionPromptText->SetText(PromptText);
			InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UYomiMainHUDWidget::ShowBiomeTitle(FText BiomeName, FText JapaneseName)
{
	if (BiomeNameText)
	{
		BiomeNameText->SetText(BiomeName);
		BiomeNameText->SetVisibility(ESlateVisibility::Visible);
	}
	if (BiomeJapaneseText)
	{
		BiomeJapaneseText->SetText(JapaneseName);
		BiomeJapaneseText->SetVisibility(ESlateVisibility::Visible);
	}
	BiomeTitleTimer = 5.0f;
}

void UYomiMainHUDWidget::UpdateHotbarSlot(int32 SlotIndex, FName ItemID, int32 Quantity)
{
	// In full implementation, update the hotbar slot UI element
}

void UYomiMainHUDWidget::SetSelectedHotbarSlot(int32 SlotIndex)
{
	// In full implementation, highlight the selected slot
}

void UYomiMainHUDWidget::UpdateFoodBuffDisplay(int32 SlotIndex, FText FoodName, float RemainingDuration)
{
	// In full implementation, show active food buff icons with duration
}
