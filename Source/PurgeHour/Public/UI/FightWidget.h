// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidgetBase.h"
#include "FightWidget.generated.h"

class UTextBlock;
class UProgressBar;
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
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	// 缓存最大血量用于计算百分比
	float MaxHealth = 100.f;

	// 动态委托回调必须是 UFUNCTION
	UFUNCTION()
	void UpdateWeaponName(const FText& NewWeaponName);
	UFUNCTION()
	void UpdateCurrentBullets(int32 NewBulletNum);
	UFUNCTION()
	void UpdateReserveAmmo(int32 NewReserveAmmo);
	UFUNCTION()
	void UpdateHealth(float NewHealth);
	UFUNCTION()
	void UpdateMaxHealth(float NewMaxHealth);
};
