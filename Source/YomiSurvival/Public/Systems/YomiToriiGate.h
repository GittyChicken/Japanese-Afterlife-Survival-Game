// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiToriiGate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UNiagaraComponent;

/**
 * Torii Gate - Portal system for fast travel (equivalent to Valheim's portals).
 * Players build pairs of torii gates and name them to create linked portals.
 * Certain resources cannot be teleported (like valuable ores).
 */
UCLASS()
class YOMISURVIVAL_API AYomiToriiGate : public AActor
{
	GENERATED_BODY()

public:
	AYomiToriiGate();

	/** Set the portal tag for linking gates. */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetPortalTag(const FText& InTag);

	UFUNCTION(BlueprintPure, Category = "Portal")
	FText GetPortalTag() const { return PortalTag; }

	/** Check if this gate is linked to another. */
	UFUNCTION(BlueprintPure, Category = "Portal")
	bool IsLinked() const { return LinkedGate != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Portal")
	AYomiToriiGate* GetLinkedGate() const { return LinkedGate; }

	/** Attempt to teleport an actor through this gate. */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool TeleportActor(AActor* ActorToTeleport);

	/** Check if an actor can teleport (doesn't carry restricted items). */
	UFUNCTION(BlueprintPure, Category = "Portal")
	bool CanActorTeleport(AActor* Actor) const;

	/** Activate/deactivate the portal effect. */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetPortalActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Portal")
	bool IsPortalActive() const { return bIsActive; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> GateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TeleportZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> PortalEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FText PortalTag;

private:
	UPROPERTY()
	TObjectPtr<AYomiToriiGate> LinkedGate;

	bool bIsActive = false;
	float TeleportCooldown = 3.0f;
	float LastTeleportTime = -100.0f;

	void FindLinkedGate();
	void RegisterGate();
	void UnregisterGate();

	// Items that cannot be teleported through gates
	static TArray<EResourceType> RestrictedResources;
};
