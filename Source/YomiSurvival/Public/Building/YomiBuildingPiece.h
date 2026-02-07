// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/YomiGameTypes.h"
#include "YomiBuildingPiece.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * A placed building piece in the world.
 * Supports damage, repair, snapping, comfort radius, and weather protection.
 * Types include walls, floors, roofs, doors, fences, shrines, torii gates, etc.
 */
UCLASS()
class YOMISURVIVAL_API AYomiBuildingPiece : public AActor
{
	GENERATED_BODY()

public:
	AYomiBuildingPiece();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void InitializePiece(const FBuildingPieceData& InData);

	UFUNCTION(BlueprintPure, Category = "Building")
	const FBuildingPieceData& GetPieceData() const { return PieceData; }

	UFUNCTION(BlueprintPure, Category = "Building")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Building")
	float GetHealthPercent() const { return PieceData.MaxHealth > 0.0f ? CurrentHealth / PieceData.MaxHealth : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Building")
	float TakeDamage(float DamageAmount, EDamageType DamageType);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void Repair(float Amount = -1.0f);

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsDestroyed() const { return CurrentHealth <= 0.0f; }

	/** Ghost mode for placement preview. */
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetAsGhost(bool bGhost);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetGhostValid(bool bValid);

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsGhost() const { return bIsGhost; }

	/** Get snap points for other pieces to connect to. */
	UFUNCTION(BlueprintPure, Category = "Building")
	TArray<FVector> GetSnapPoints() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FBuildingPieceData PieceData;

	UPROPERTY(BlueprintReadOnly, Category = "Building", Replicated)
	float CurrentHealth;

	bool bIsGhost = false;

	UPROPERTY(EditAnywhere, Category = "Building|Materials")
	TObjectPtr<UMaterialInterface> GhostMaterialValid;

	UPROPERTY(EditAnywhere, Category = "Building|Materials")
	TObjectPtr<UMaterialInterface> GhostMaterialInvalid;

	TObjectPtr<UMaterialInterface> OriginalMaterial;

	void OnPieceDestroyed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
