// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GA/BaseGA.h"
#include "BaseWeaponGA.generated.h"

class AWeaponBase;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API UBaseWeaponGA : public UBaseGA
{
	GENERATED_BODY()
public:
	UBaseWeaponGA();
	
	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Get")
	AWeaponBase* GetCurrentWeapon() ;
	
	
	
};
