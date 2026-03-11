// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GA/BaseGA.h"
#include "BaseSwordGA.generated.h"

class ASwordBase;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API UBaseSwordGA : public UBaseGA
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Get")
	ASwordBase* GetCurrentWeapon();
	
};
