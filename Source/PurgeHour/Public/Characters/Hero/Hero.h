// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "PureData/HeroState.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Hero.generated.h"
class AWeaponBase;
class UWeaponData;
class USpringArmComponent;
class UCameraComponent;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHero : public ACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AHero();
	
	//动画
	UPROPERTY(EditDefaultsOnly,Category="Anim")
	TObjectPtr<UAnimMontage> FireMontage;
protected:
	virtual void BeginPlay() override;
	void Init();
	
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
	
	//空弹夹音效
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> EmptyMagazineSound;
	
	//GAS
	//初始化GE
	UPROPERTY(EditDefaultsOnly,Category="GE")
	TSubclassOf<UGameplayEffect> InitGE;
	
	void InitGEToSelf();
	
	//GA
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "GA")
	FGameplayAbilitySpecHandle GAFireHandle;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "GA")
	FGameplayAbilitySpecHandle GAReloadHandle;
	
public:
	virtual void Tick(float DeltaTime) override;
	//捡起武器相关
	void PickUpWeapon(AWeaponBase* NewWeapon);
	//状态相关
	UFUNCTION(BlueprintCallable, Category = "Hero")
	FORCEINLINE void SetCurrentHeroState(EHeroState NewState) { CurrentHeroState = NewState; }
	UFUNCTION(BlueprintCallable, Category = "Hero")
	FORCEINLINE EHeroState GetCurrentHeroState() { return CurrentHeroState; }
	
	//开火相关
	// 是否正在开火（已整合到GA，不再经过武器判断）
	UPROPERTY(BlueprintReadWrite, Category = "Hero")
	bool bIsFiring = false;
	
	void Fire();
	void StopFire();
	void Reload();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")	
	void AimFocusOpen();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")
	void AimFocusClose();
	
	//后坐力
	UFUNCTION(BlueprintCallable, Category = "Offset")
	void AddPitchOffset(float PitchOffsetAmount);
	UFUNCTION(BlueprintCallable, Category = "Offset")
	void AddYawOffset(float YawOffsetAmount);
	
	//获取当前武器
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Hero")
	FORCEINLINE AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
};
