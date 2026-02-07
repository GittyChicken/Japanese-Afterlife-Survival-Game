// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/YomiGameTypes.h"
#include "YomiGameState.generated.h"

/**
 * Game state tracking world-wide progression, boss status, active players,
 * and the shared discovery of biomes and shrines.
 */
UCLASS()
class YOMISURVIVAL_API AYomiGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AYomiGameState();

	// ========================================================================
	// WORLD PROGRESSION
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "GameState")
	TArray<EBossType> GetDefeatedBosses() const { return DefeatedBosses; }

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void MarkBossDefeated(EBossType Boss);

	UFUNCTION(BlueprintPure, Category = "GameState")
	bool IsBossDefeated(EBossType Boss) const;

	// ========================================================================
	// SHRINE NETWORK
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "GameState|Shrines")
	void RegisterShrine(FName ShrineID, FVector Location, FText ShrineName);

	UFUNCTION(BlueprintPure, Category = "GameState|Shrines")
	TArray<FName> GetDiscoveredShrines() const;

	UFUNCTION(BlueprintPure, Category = "GameState|Shrines")
	FVector GetShrineLocation(FName ShrineID) const;

	// ========================================================================
	// WORLD TIME
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "GameState")
	int32 GetWorldDay() const { return WorldDay; }

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void AdvanceDay();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	TArray<EBossType> DefeatedBosses;

	UPROPERTY(Replicated)
	int32 WorldDay = 1;

	struct FShrineInfo
	{
		FVector Location;
		FText DisplayName;
	};
	TMap<FName, FShrineInfo> ShrineRegistry;
};
