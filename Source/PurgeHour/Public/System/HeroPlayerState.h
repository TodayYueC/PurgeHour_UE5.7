// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "HeroPlayerState.generated.h"

struct FOnAttributeChangeData;

class UHeroYueASC;
class UHeroAttributeSet;

// UI 广播委托（蓝图可绑定）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedDelegate, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedDelegate, int32, NewAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxAmmoChangedDelegate, int32, NewMaxAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReserveAmmoChangedDelegate, int32, NewReserveAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponNameChangedDelegate, const FText&, NewWeaponName);

/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHeroPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AHeroPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UHeroAttributeSet* GetAttributeSet() const{ return AttributeSet;}

	virtual void BeginPlay() override;

	// UI 委托（蓝图可绑定）
	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnHealthChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnMaxHealthChangedDelegate OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnAmmoChangedDelegate OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnMaxAmmoChangedDelegate OnMaxAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnReserveAmmoChangedDelegate OnReserveAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnWeaponNameChangedDelegate OnWeaponNameChanged;

	// 由 WeaponBase 捡枪时调用，广播武器名
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BroadcastWeaponName(const FText& WeaponName);

	// 由 WeaponBase 捡枪时调用，广播当前弹药数量到 UI
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BroadcastAmmo();
	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UHeroAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UHeroYueASC> AbilitySystemComponent;

private:
	// 注册属性变化回调
	void BindAttributeCallbacks();

	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void OnAmmoAttributeChanged(const FOnAttributeChangeData& Data);
	void OnMaxAmmoAttributeChanged(const FOnAttributeChangeData& Data);
	void OnReserveAmmoAttributeChanged(const FOnAttributeChangeData& Data);
};
