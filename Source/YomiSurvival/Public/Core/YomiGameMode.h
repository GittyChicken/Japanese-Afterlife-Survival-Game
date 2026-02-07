// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/YomiGameTypes.h"
#include "YomiGameMode.generated.h"

class AYomiBiomeManager;
class AYomiPlayerCharacter;

/**
 * Main game mode for Yomi Survival.
 * Manages world generation, player spawning, boss progression,
 * multiplayer sessions, and the overall game loop.
 *
 * Game Loop: Explore -> Gather -> Craft -> Fight Boss -> Unlock New Tier -> Repeat
 */
UCLASS()
class YOMISURVIVAL_API AYomiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AYomiGameMode();

	// ========================================================================
	// WORLD MANAGEMENT
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Game")
	void GenerateNewWorld(int32 Seed = -1);

	UFUNCTION(BlueprintPure, Category = "Game")
	AYomiBiomeManager* GetBiomeManager() const { return BiomeManager; }

	UFUNCTION(BlueprintPure, Category = "Game")
	int32 GetWorldSeed() const { return WorldSeed; }

	// ========================================================================
	// PLAYER MANAGEMENT
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RespawnPlayer(APlayerController* PC);

	UFUNCTION(BlueprintPure, Category = "Game")
	FVector GetRespawnLocation(APlayerController* PC) const;

	// ========================================================================
	// BOSS PROGRESSION
	// ========================================================================

	/** Notify the game that a boss has been defeated. */
	UFUNCTION(BlueprintCallable, Category = "Game|Progression")
	void NotifyBossDefeated(EBossType Boss, AYomiPlayerCharacter* Defeater);

	/** Get which bosses have been defeated in this world. */
	UFUNCTION(BlueprintPure, Category = "Game|Progression")
	TArray<EBossType> GetWorldDefeatedBosses() const { return WorldDefeatedBosses; }

	/** Check if a specific biome is unlocked for the world. */
	UFUNCTION(BlueprintPure, Category = "Game|Progression")
	bool IsBiomeUnlocked(EYomiBiome Biome) const;

	// ========================================================================
	// DIFFICULTY SCALING
	// ========================================================================

	/** Get the current difficulty multiplier. Scales with player count and progression. */
	UFUNCTION(BlueprintPure, Category = "Game")
	float GetDifficultyMultiplier() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float BaseDifficultyMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float DifficultyPerPlayer = 0.5f;

protected:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	TSubclassOf<AYomiBiomeManager> BiomeManagerClass;

private:
	UPROPERTY()
	TObjectPtr<AYomiBiomeManager> BiomeManager;

	int32 WorldSeed = 0;
	TArray<EBossType> WorldDefeatedBosses;

	// Player respawn points (shrine-based)
	TMap<APlayerController*, FVector> PlayerRespawnPoints;
};
