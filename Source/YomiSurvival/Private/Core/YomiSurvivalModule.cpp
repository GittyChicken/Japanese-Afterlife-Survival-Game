// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Core/YomiSurvivalModule.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FYomiSurvivalModule, YomiSurvival, "YomiSurvival");

void FYomiSurvivalModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("YomiSurvival Module Started - Meido: Path of Shadows"));
}

void FYomiSurvivalModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("YomiSurvival Module Shutdown"));
}
