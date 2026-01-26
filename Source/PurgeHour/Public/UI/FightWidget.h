// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidgetBase.h"
#include "FightWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API UFightWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
protected:
	
	virtual auto NativeConstruct() -> void override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentBullets;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AllBullets;
	
	void UpdateWeaponName(const FText& NewWeaponName);
	void UpdateCurrentBullets(int32 NewBulletNum);
	void UpdateAllBullets(int32 NewAllBulletNum);
};
