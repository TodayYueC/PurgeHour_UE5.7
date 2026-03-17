// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBehaviorTree;
struct FAIStimulus;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIController();
	
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree>BehaviorTree;
	
	UPROPERTY(VisibleAnywhere, Category="AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	UPROPERTY(VisibleAnywhere, Category="AI")
	TObjectPtr<UAISenseConfig_Sight> Sight;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
};
