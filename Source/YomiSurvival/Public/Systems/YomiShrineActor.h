// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiShrineActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

/**
 * Shrine actor - provides meditation spot, offering system, and fast travel.
 * Players can meditate at shrines for health/Ki restoration and buffs.
 * Offerings at shrines grant temporary blessings from kami.
 */
UCLASS()
class YOMISURVIVAL_API AYomiShrineActor : public AActor
{
	GENERATED_BODY()

public:
	AYomiShrineActor();

	/** Make an offering to the shrine. */
	UFUNCTION(BlueprintCallable, Category = "Shrine")
	bool MakeOffering(FName ItemID, int32 Quantity);

	/** Get the current blessing type from offerings. */
	UFUNCTION(BlueprintPure, Category = "Shrine")
	FText GetCurrentBlessing() const { return CurrentBlessing; }

	/** Register this shrine for the fast travel network. */
	UFUNCTION(BlueprintCallable, Category = "Shrine")
	void RegisterForFastTravel(FText ShrineName);

	UFUNCTION(BlueprintPure, Category = "Shrine")
	FText GetShrineName() const { return ShrineName; }

	UFUNCTION(BlueprintPure, Category = "Shrine")
	bool IsDiscovered() const { return bDiscovered; }

	UFUNCTION(BlueprintCallable, Category = "Shrine")
	void Discover();

	/** Set this as a respawn point. */
	UFUNCTION(BlueprintCallable, Category = "Shrine")
	void SetAsRespawnPoint(AActor* Player);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInteractionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ShrineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> InteractionZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> SpiritFlameEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine")
	FText ShrineName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine")
	EYomiBiome BiomeLocation = EYomiBiome::None;

private:
	bool bDiscovered = false;
	FText CurrentBlessing;

	// Offering thresholds for different blessings
	int32 TotalOfferingValue = 0;
};
