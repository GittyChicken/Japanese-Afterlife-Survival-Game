// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiBiomeZone.generated.h"

class USphereComponent;
class UNiagaraComponent;
class AYomiResourceNode;
class AYomiEnemyBase;

/**
 * Represents a single biome island in the world of Meido.
 * Handles resource node spawning, enemy spawning, ambient effects,
 * and biome-specific environmental features.
 */
UCLASS()
class YOMISURVIVAL_API AYomiBiomeZone : public AActor
{
	GENERATED_BODY()

public:
	AYomiBiomeZone();

	UFUNCTION(BlueprintCallable, Category = "Biome")
	void InitializeBiome(EYomiBiome InBiomeType, const FBiomeData& InBiomeData, float InRadius);

	UFUNCTION(BlueprintPure, Category = "Biome")
	EYomiBiome GetBiomeType() const { return BiomeType; }

	UFUNCTION(BlueprintPure, Category = "Biome")
	const FBiomeData& GetBiomeData() const { return BiomeData; }

	UFUNCTION(BlueprintPure, Category = "Biome")
	float GetRadius() const { return ZoneRadius; }

	/** Check if a point is within this biome zone. */
	UFUNCTION(BlueprintPure, Category = "Biome")
	bool IsPointInZone(FVector Point) const;

	/** Spawn resource nodes in this biome. */
	UFUNCTION(BlueprintCallable, Category = "Biome")
	void SpawnResources();

	/** Spawn enemies in this biome. */
	UFUNCTION(BlueprintCallable, Category = "Biome")
	void SpawnEnemies();

	/** Called when a player enters this biome zone. */
	UFUNCTION(BlueprintCallable, Category = "Biome")
	void OnPlayerEnterZone(ACharacter* Player);

	/** Called when a player exits this biome zone. */
	UFUNCTION(BlueprintCallable, Category = "Biome")
	void OnPlayerExitZone(ACharacter* Player);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> ZoneTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> AmbientEffects;

private:
	EYomiBiome BiomeType = EYomiBiome::None;
	FBiomeData BiomeData;
	float ZoneRadius = 5000.0f;

	UPROPERTY()
	TArray<AYomiResourceNode*> ResourceNodes;

	UPROPERTY()
	TArray<AYomiEnemyBase*> SpawnedEnemies;

	UPROPERTY(EditAnywhere, Category = "Biome|Spawning")
	int32 MaxResourceNodes = 50;

	UPROPERTY(EditAnywhere, Category = "Biome|Spawning")
	int32 MaxEnemies = 20;

	UPROPERTY(EditAnywhere, Category = "Biome|Spawning")
	float EnemyRespawnTime = 300.0f;

	FVector GetRandomPointInZone() const;
};
