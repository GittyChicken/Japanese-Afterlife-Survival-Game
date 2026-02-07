// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/YomiCharacterBase.h"
#include "Core/YomiGameTypes.h"
#include "YomiCompanion.generated.h"

class AYomiPlayerCharacter;

/**
 * Spirit companion that can be tamed and follow the player.
 * Companions: Kitsune, Tanuki, Kodama, Spirit Wolf, Crane Spirit.
 * Each has unique passive abilities and assists in exploration/combat.
 */
UCLASS()
class YOMISURVIVAL_API AYomiCompanion : public AYomiCharacterBase
{
	GENERATED_BODY()

public:
	AYomiCompanion();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void InitializeCompanion(ECompanionType InType, AYomiPlayerCharacter* InOwner);

	UFUNCTION(BlueprintPure, Category = "Companion")
	ECompanionType GetCompanionType() const { return CompanionType; }

	UFUNCTION(BlueprintPure, Category = "Companion")
	AYomiPlayerCharacter* GetCompanionOwner() const { return CompanionOwner; }

	UFUNCTION(BlueprintPure, Category = "Companion")
	FText GetCompanionName() const { return CompanionName; }

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void SetCompanionName(const FText& InName) { CompanionName = InName; }

	/** Called when companion detects a resource nearby. */
	UFUNCTION(BlueprintCallable, Category = "Companion")
	void OnResourceDetected(AActor* Resource);

	/** Called when companion detects an enemy nearby. */
	UFUNCTION(BlueprintCallable, Category = "Companion")
	void OnEnemyDetected(AActor* Enemy);

	/** Activate the companion's passive ability. */
	UFUNCTION(BlueprintCallable, Category = "Companion")
	void ActivatePassiveAbility();

	/** Feed the companion to increase loyalty. */
	UFUNCTION(BlueprintCallable, Category = "Companion")
	void Feed(FName FoodItemID);

	UFUNCTION(BlueprintPure, Category = "Companion")
	float GetLoyalty() const { return Loyalty; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	ECompanionType CompanionType = ECompanionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	FText CompanionName;

	UPROPERTY()
	TObjectPtr<AYomiPlayerCharacter> CompanionOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float FollowDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float Loyalty = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float MaxLoyalty = 100.0f;

private:
	void FollowOwner(float DeltaTime);
	void ScanForPointsOfInterest();
	float ScanTimer = 0.0f;
	float ScanInterval = 3.0f;
};
