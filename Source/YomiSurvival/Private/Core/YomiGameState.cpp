// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Core/YomiGameState.h"
#include "Net/UnrealNetwork.h"

AYomiGameState::AYomiGameState()
{
}

void AYomiGameState::MarkBossDefeated(EBossType Boss)
{
	if (!DefeatedBosses.Contains(Boss))
	{
		DefeatedBosses.Add(Boss);
		UE_LOG(LogYomi, Log, TEXT("World state: Boss %d defeated"), static_cast<uint8>(Boss));
	}
}

bool AYomiGameState::IsBossDefeated(EBossType Boss) const
{
	return DefeatedBosses.Contains(Boss);
}

void AYomiGameState::RegisterShrine(FName ShrineID, FVector Location, FText ShrineName)
{
	FShrineInfo Info;
	Info.Location = Location;
	Info.DisplayName = ShrineName;
	ShrineRegistry.Add(ShrineID, Info);

	UE_LOG(LogYomi, Log, TEXT("Shrine registered: %s at %s"), *ShrineID.ToString(), *Location.ToString());
}

TArray<FName> AYomiGameState::GetDiscoveredShrines() const
{
	TArray<FName> ShrineIDs;
	ShrineRegistry.GetKeys(ShrineIDs);
	return ShrineIDs;
}

FVector AYomiGameState::GetShrineLocation(FName ShrineID) const
{
	if (const FShrineInfo* Info = ShrineRegistry.Find(ShrineID))
	{
		return Info->Location;
	}
	return FVector::ZeroVector;
}

void AYomiGameState::AdvanceDay()
{
	WorldDay++;
	UE_LOG(LogYomi, Log, TEXT("Day %d in Meido"), WorldDay);
}

void AYomiGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AYomiGameState, DefeatedBosses);
	DOREPLIFETIME(AYomiGameState, WorldDay);
}
