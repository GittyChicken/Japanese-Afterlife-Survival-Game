// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "UI/YomiHUD.h"
#include "UI/YomiMainHUDWidget.h"

AYomiHUD::AYomiHUD()
{
}

void AYomiHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UYomiMainHUDWidget>(GetOwningPlayerController(), MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}

void AYomiHUD::ShowBossHealthBar(FText BossName, float HealthPercent)
{
	if (MainHUDWidget)
	{
		MainHUDWidget->ShowBossBar(BossName, HealthPercent);
	}
}

void AYomiHUD::HideBossHealthBar()
{
	if (MainHUDWidget)
	{
		MainHUDWidget->HideBossBar();
	}
}

void AYomiHUD::ShowInteractionPrompt(FText PromptText)
{
	if (MainHUDWidget)
	{
		MainHUDWidget->SetInteractionPrompt(PromptText);
	}
}

void AYomiHUD::HideInteractionPrompt()
{
	if (MainHUDWidget)
	{
		MainHUDWidget->SetInteractionPrompt(FText::GetEmpty());
	}
}

void AYomiHUD::ShowNotification(FText Message, float Duration)
{
	if (MainHUDWidget)
	{
		MainHUDWidget->DisplayNotification(Message, Duration);
	}
}

void AYomiHUD::ShowBiomeTransition(FText BiomeName, FText JapaneseName)
{
	if (MainHUDWidget)
	{
		MainHUDWidget->ShowBiomeTitle(BiomeName, JapaneseName);
	}
}

void AYomiHUD::ToggleInventoryScreen()
{
	UE_LOG(LogYomi, Log, TEXT("Toggle inventory screen"));
}

void AYomiHUD::ToggleCraftingScreen(ECraftingStation Station)
{
	UE_LOG(LogYomi, Log, TEXT("Toggle crafting screen for station: %d"), static_cast<uint8>(Station));
}

void AYomiHUD::ToggleMapScreen()
{
	UE_LOG(LogYomi, Log, TEXT("Toggle map screen"));
}
