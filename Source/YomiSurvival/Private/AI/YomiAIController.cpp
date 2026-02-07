// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

#include "AI/YomiAIController.h"
#include "AI/YomiEnemyBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

AYomiAIController::AYomiAIController()
{
	// Setup perception
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1000.0f;
	HearingConfig->SetMaxAge(3.0f);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
}

void AYomiAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent())
	{
		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AYomiAIController::OnPerceptionUpdated);
	}
}

void AYomiAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PatrolCenter = InPawn->GetActorLocation();

	AYomiEnemyBase* Enemy = Cast<AYomiEnemyBase>(InPawn);
	if (Enemy)
	{
		PatrolRadius = Enemy->GetAggroRange();

		// Start behavior tree if available
		// RunBehaviorTree(Enemy->BehaviorTree);
	}
}

void AYomiAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	AYomiEnemyBase* ControlledEnemy = Cast<AYomiEnemyBase>(GetPawn());
	if (!ControlledEnemy) return;

	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		GetPerceptionComponent()->GetActorsPerception(Actor, Info);

		for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				ControlledEnemy->OnPlayerDetected(Actor);
			}
			else
			{
				ControlledEnemy->OnPlayerLost();
			}
		}
	}
}

void AYomiAIController::SetPatrolCenter(FVector Center, float Radius)
{
	PatrolCenter = Center;
	PatrolRadius = Radius;
}

FVector AYomiAIController::GetRandomPatrolPoint() const
{
	FNavLocation NavResult;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSys)
	{
		if (NavSys->GetRandomReachablePointInRadius(PatrolCenter, PatrolRadius, NavResult))
		{
			return NavResult.Location;
		}
	}

	// Fallback - random point in circle
	float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
	float Distance = FMath::FRandRange(0.0f, PatrolRadius);
	return PatrolCenter + FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.0f);
}
