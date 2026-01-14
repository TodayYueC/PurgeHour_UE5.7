// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FightHUD.generated.h"
class UFightWidget;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API AFightHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	//战斗面板UI
	UPROPERTY(EditDefaultsOnly,Category="UI")
	TSubclassOf<UFightWidget> FightWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UFightWidget> FightWidget;
	
	
};
