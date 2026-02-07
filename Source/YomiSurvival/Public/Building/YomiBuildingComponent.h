// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/YomiGameTypes.h"
#include "YomiBuildingComponent.generated.h"

class AYomiBuildingPiece;
class UYomiInventoryComponent;

/**
 * Component that handles the building system for the player.
 * Manages build mode, ghost placement, snapping, and construction.
 * Supports traditional Japanese architecture: minka, machiya, castles, shrines.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOMISURVIVAL_API UYomiBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UYomiBuildingComponent();

	// ========================================================================
	// BUILD MODE
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Building")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void EnterBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void ExitBuildMode();

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsInBuildMode() const { return bInBuildMode; }

	// ========================================================================
	// PIECE SELECTION
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SelectBuildingPiece(FName PieceID);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void CycleBuildingPieceForward();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void CycleBuildingPieceBackward();

	UFUNCTION(BlueprintPure, Category = "Building")
	FName GetSelectedPieceID() const { return SelectedPieceID; }

	UFUNCTION(BlueprintPure, Category = "Building")
	FBuildingPieceData GetSelectedPieceData() const;

	// ========================================================================
	// PLACEMENT
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Building")
	void RotatePiece(float Degrees);

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool CanPlaceAtCurrentLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool PlacePiece();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void UpdateGhostPlacement();

	// ========================================================================
	// DEMOLITION
	// ========================================================================

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool DemolishPiece(AYomiBuildingPiece* Piece);

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool RepairPiece(AYomiBuildingPiece* Piece);

	// ========================================================================
	// DELEGATES
	// ========================================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildModeChanged, bool, bEnabled);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPiecePlaced, AYomiBuildingPiece*, PlacedPiece);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuildModeChanged OnBuildModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPiecePlaced OnPiecePlaced;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bInBuildMode = false;
	FName SelectedPieceID;
	float CurrentRotation = 0.0f;

	UPROPERTY()
	TObjectPtr<AYomiBuildingPiece> GhostPiece;

	UPROPERTY()
	TObjectPtr<UYomiInventoryComponent> OwnerInventory;

	UPROPERTY(EditAnywhere, Category = "Building")
	float PlacementDistance = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Building")
	float SnapDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Building")
	TObjectPtr<UDataTable> BuildingPieceDataTable;

	// Available pieces for building (cached)
	TArray<FName> AvailablePieceIDs;
	int32 CurrentPieceIndex = 0;

	FVector GetPlacementLocation() const;
	FRotator GetPlacementRotation() const;
	bool CheckPlacementCollision(FVector Location) const;
	AYomiBuildingPiece* FindNearestSnapPoint(FVector Location) const;

	void CreateGhostPiece();
	void DestroyGhostPiece();
};
