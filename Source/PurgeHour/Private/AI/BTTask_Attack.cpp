// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/Enemy/EnemyBase1.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Enemy Attack";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AEnemyBase1* Enemy = AIC ? Cast<AEnemyBase1>(AIC->GetPawn()) : nullptr;

	if (Enemy)
	{
		Enemy->Attack();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
