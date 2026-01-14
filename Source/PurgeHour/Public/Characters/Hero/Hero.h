// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "PureData/HeroState.h"
#include "Hero.generated.h"
class AWeaponBase;
class UWeaponData;
class USpringArmComponent;
class UCameraComponent;


/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHero : public ACharacterBase
{
	GENERATED_BODY()
public:
	AHero();
	
	//动画
	UPROPERTY(EditDefaultsOnly,Category="Anim")
	TObjectPtr<UAnimMontage> FireMontage;
protected:
	virtual void BeginPlay() override;
	
	
	//摄像机与弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	
	//当前持有武器
	UPROPERTY()
	AWeaponBase* CurrentWeapon;
	
	//当前状态
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	EHeroState CurrentHeroState;
	

	
public:
	virtual void Tick(float DeltaTime) override;
	//捡起武器相关
	void PickUpWeapon(AWeaponBase* NewWeapon);
	//状态相关
	UFUNCTION(BlueprintCallable, Category = "Hero")
	FORCEINLINE void SetCurrentHeroState(EHeroState NewState) { CurrentHeroState = NewState; }
	UFUNCTION(BlueprintCallable, Category = "Hero")
	FORCEINLINE EHeroState GetCurrentHeroState() { return CurrentHeroState; }
	
	//开火与停止开火相关
	void Fire();
	void StopFire();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")	
	void AimFocusOpen();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")
	void AimFocusClose();
};
