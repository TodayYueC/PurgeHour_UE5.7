// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EnemyAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionTypes.h"
#include "Characters/Hero/Hero.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

namespace
{
bool IsPlayerTarget(const AActor* Actor, const UWorld* World)
{
	if (!Actor)
	{
		return false;
	}

	if (Actor->IsA<AHero>() || Actor->ActorHasTag(TEXT("Player")) || Actor->ActorHasTag(TEXT("Hero")))
	{
		return true;
	}

	if (World)
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC->GetPawn() == Actor;
		}
	}

	return false;
}
}

AEnemyAIController::AEnemyAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	SetPerceptionComponent(*AIPerceptionComponent);
	
	if (Sight)
	{
		Sight->SightRadius = 2000.0f;
		Sight->LoseSightRadius = 2500.0f;
		
		Sight->PeripheralVisionAngleDegrees = 90.0f;
		Sight->DetectionByAffiliation.bDetectEnemies = true;
		Sight->DetectionByAffiliation.bDetectNeutrals = true;
		Sight->DetectionByAffiliation.bDetectFriendlies = true;
	}
	AIPerceptionComponent->ConfigureSense(*Sight);
	
	AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
	
	
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
	
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	
	
	
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !GetBlackboardComponent())
	{
		return;
	}

	if (!IsPlayerTarget(Actor, GetWorld()))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("See Player"));
		}
		return;
	}

	if (GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")) == Actor)
	{
		GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Lost Player"));
		}
	}
}
