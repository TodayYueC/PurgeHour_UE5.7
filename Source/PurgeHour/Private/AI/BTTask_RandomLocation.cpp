// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_RandomLocation::UBTTask_RandomLocation()
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_RandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIC?AIC->GetPawn():nullptr;
	
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	if (NavSys)
	{
		FNavLocation RandomLocation;
		bool bFound = NavSys->GetRandomPointInNavigableRadius(
			AIPawn->GetActorLocation(),
			Radius,
			RandomLocation
		);
		if (bFound)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(
				GetSelectedBlackboardKey(),
				RandomLocation.Location
			);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
