// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DelegatesSubsystem.generated.h"

DECLARE_DELEGATE_OneParam(FWeaponNameChangedDelegate, const FText&)
/**
 * 
 */
UCLASS()
class PURGEHOUR_API UDelegatesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FWeaponNameChangedDelegate WeaponNameChangedDelegate;
};
