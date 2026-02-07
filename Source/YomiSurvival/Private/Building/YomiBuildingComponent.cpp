// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "Building/YomiBuildingComponent.h"
#include "Building/YomiBuildingPiece.h"
#include "Inventory/YomiInventoryComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

UYomiBuildingComponent::UYomiBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UYomiBuildingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerInventory = GetOwner()->FindComponentByClass<UYomiInventoryComponent>();

	// Load available building pieces from data table
	if (BuildingPieceDataTable)
	{
		TArray<FBuildingPieceData*> Rows;
		BuildingPieceDataTable->GetAllRows(TEXT("BuildingPieces"), Rows);

		for (const FBuildingPieceData* Row : Rows)
		{
			if (Row)
			{
				AvailablePieceIDs.Add(Row->PieceID);
			}
		}
	}
}

void UYomiBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bInBuildMode)
	{
		UpdateGhostPlacement();
	}
}

// ============================================================================
// BUILD MODE
// ============================================================================

void UYomiBuildingComponent::ToggleBuildMode()
{
	if (bInBuildMode)
		ExitBuildMode();
	else
		EnterBuildMode();
}

void UYomiBuildingComponent::EnterBuildMode()
{
	bInBuildMode = true;
	SetComponentTickEnabled(true);

	if (AvailablePieceIDs.Num() > 0)
	{
		SelectBuildingPiece(AvailablePieceIDs[0]);
	}

	OnBuildModeChanged.Broadcast(true);
	UE_LOG(LogYomiBuilding, Log, TEXT("Entered build mode"));
}

void UYomiBuildingComponent::ExitBuildMode()
{
	bInBuildMode = false;
	SetComponentTickEnabled(false);
	DestroyGhostPiece();

	OnBuildModeChanged.Broadcast(false);
	UE_LOG(LogYomiBuilding, Log, TEXT("Exited build mode"));
}

// ============================================================================
// PIECE SELECTION
// ============================================================================

void UYomiBuildingComponent::SelectBuildingPiece(FName PieceID)
{
	SelectedPieceID = PieceID;
	DestroyGhostPiece();
	CreateGhostPiece();

	UE_LOG(LogYomiBuilding, Log, TEXT("Selected building piece: %s"), *PieceID.ToString());
}

void UYomiBuildingComponent::CycleBuildingPieceForward()
{
	if (AvailablePieceIDs.Num() == 0) return;
	CurrentPieceIndex = (CurrentPieceIndex + 1) % AvailablePieceIDs.Num();
	SelectBuildingPiece(AvailablePieceIDs[CurrentPieceIndex]);
}

void UYomiBuildingComponent::CycleBuildingPieceBackward()
{
	if (AvailablePieceIDs.Num() == 0) return;
	CurrentPieceIndex = (CurrentPieceIndex - 1 + AvailablePieceIDs.Num()) % AvailablePieceIDs.Num();
	SelectBuildingPiece(AvailablePieceIDs[CurrentPieceIndex]);
}

FBuildingPieceData UYomiBuildingComponent::GetSelectedPieceData() const
{
	if (BuildingPieceDataTable && !SelectedPieceID.IsNone())
	{
		TArray<FBuildingPieceData*> Rows;
		BuildingPieceDataTable->GetAllRows(TEXT("BuildingPieces"), Rows);

		for (const FBuildingPieceData* Row : Rows)
		{
			if (Row && Row->PieceID == SelectedPieceID)
			{
				return *Row;
			}
		}
	}
	return FBuildingPieceData();
}

// ============================================================================
// PLACEMENT
// ============================================================================

void UYomiBuildingComponent::RotatePiece(float Degrees)
{
	CurrentRotation = FMath::Fmod(CurrentRotation + Degrees, 360.0f);
}

bool UYomiBuildingComponent::CanPlaceAtCurrentLocation() const
{
	if (SelectedPieceID.IsNone()) return false;
	if (!OwnerInventory) return false;

	FBuildingPieceData Data = GetSelectedPieceData();

	// Check resources
	if (!OwnerInventory->HasResources(Data.BuildCost)) return false;

	// Check collision
	FVector Location = GetPlacementLocation();
	if (!CheckPlacementCollision(Location)) return false;

	return true;
}

bool UYomiBuildingComponent::PlacePiece()
{
	if (!CanPlaceAtCurrentLocation()) return false;

	FBuildingPieceData Data = GetSelectedPieceData();

	// Consume resources
	if (!OwnerInventory->ConsumeResources(Data.BuildCost)) return false;

	// Spawn the actual building piece
	FVector Location = GetPlacementLocation();
	FRotator Rotation = GetPlacementRotation();

	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AYomiBuildingPiece* NewPiece = GetWorld()->SpawnActor<AYomiBuildingPiece>(
			AYomiBuildingPiece::StaticClass(), Location, Rotation, SpawnParams);

		if (NewPiece)
		{
			NewPiece->InitializePiece(Data);
			OnPiecePlaced.Broadcast(NewPiece);

			UE_LOG(LogYomiBuilding, Log, TEXT("Placed building piece: %s at %s"),
				*Data.DisplayName.ToString(), *Location.ToString());
			return true;
		}
	}

	return false;
}

void UYomiBuildingComponent::UpdateGhostPlacement()
{
	if (!GhostPiece) return;

	FVector Location = GetPlacementLocation();
	FRotator Rotation = GetPlacementRotation();

	// Check for snap points
	AYomiBuildingPiece* SnapTarget = FindNearestSnapPoint(Location);
	if (SnapTarget)
	{
		// Snap to the nearest snap point
		Location = SnapTarget->GetActorLocation() + SnapTarget->GetActorForwardVector() * GetSelectedPieceData().SnapSize.X;
	}

	GhostPiece->SetActorLocationAndRotation(Location, Rotation);

	// Update ghost material based on placement validity
	bool bCanPlace = CanPlaceAtCurrentLocation();
	GhostPiece->SetGhostValid(bCanPlace);
}

// ============================================================================
// DEMOLITION
// ============================================================================

bool UYomiBuildingComponent::DemolishPiece(AYomiBuildingPiece* Piece)
{
	if (!Piece || !OwnerInventory) return false;

	// Return half the resources
	FBuildingPieceData Data = Piece->GetPieceData();
	for (const auto& Pair : Data.BuildCost)
	{
		FName ItemID = FName(*FString::Printf(TEXT("Resource_%d"), static_cast<uint8>(Pair.Key)));
		OwnerInventory->AddItem(ItemID, Pair.Value / 2);
	}

	Piece->Destroy();
	UE_LOG(LogYomiBuilding, Log, TEXT("Demolished building piece: %s"), *Data.DisplayName.ToString());
	return true;
}

bool UYomiBuildingComponent::RepairPiece(AYomiBuildingPiece* Piece)
{
	if (!Piece) return false;
	Piece->Repair();
	return true;
}

// ============================================================================
// PRIVATE HELPERS
// ============================================================================

FVector UYomiBuildingComponent::GetPlacementLocation() const
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return FVector::ZeroVector;

	// Trace from camera forward
	FVector Start = Owner->GetActorLocation() + FVector(0, 0, 100.0f);
	FVector Forward = Owner->GetControlRotation().Vector();
	FVector End = Start + Forward * PlacementDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	if (GhostPiece) Params.AddIgnoredActor(GhostPiece);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		return HitResult.Location;
	}

	return End;
}

FRotator UYomiBuildingComponent::GetPlacementRotation() const
{
	return FRotator(0.0f, CurrentRotation, 0.0f);
}

bool UYomiBuildingComponent::CheckPlacementCollision(FVector Location) const
{
	// Simple overlap check
	FCollisionShape Box = FCollisionShape::MakeBox(FVector(100.0f));
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (GhostPiece) Params.AddIgnoredActor(GhostPiece);

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_WorldStatic, Box, Params);
}

AYomiBuildingPiece* UYomiBuildingComponent::FindNearestSnapPoint(FVector Location) const
{
	// Find nearby building pieces for snapping
	TArray<FHitResult> Hits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SnapDistance);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(Hits, Location, Location, FQuat::Identity, ECC_WorldStatic, Sphere, Params))
	{
		for (const FHitResult& Hit : Hits)
		{
			AYomiBuildingPiece* Piece = Cast<AYomiBuildingPiece>(Hit.GetActor());
			if (Piece)
			{
				return Piece;
			}
		}
	}

	return nullptr;
}

void UYomiBuildingComponent::CreateGhostPiece()
{
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	GhostPiece = GetWorld()->SpawnActor<AYomiBuildingPiece>(
		AYomiBuildingPiece::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (GhostPiece)
	{
		GhostPiece->SetAsGhost(true);
	}
}

void UYomiBuildingComponent::DestroyGhostPiece()
{
	if (GhostPiece)
	{
		GhostPiece->Destroy();
		GhostPiece = nullptr;
	}
}
