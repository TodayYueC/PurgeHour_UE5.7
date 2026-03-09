// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGA.generated.h"

/**
 * 
 */
UCLASS()
class PURGEHOUR_API UBaseGA : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UBaseGA();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartUpTag;
	
};
