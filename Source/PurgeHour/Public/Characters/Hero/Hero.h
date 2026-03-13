// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "PureData/HeroState.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Hero.generated.h"
class AWeaponBase;
class ASwordBase;
class UWeaponData;
class USpringArmComponent;
class UCameraComponent;
class UGameplayEffect;
class UGameplayAbility;
class UMotionWarpingComponent;

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

	//当前持有的剑
	UPROPERTY(BlueprintReadOnly, Category = "Sword")
	ASwordBase* CurrentSword;
	
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
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "GA")
	FGameplayAbilitySpecHandle GAAttackHandle;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "GA")
	FGameplayAbilitySpecHandle GAComboHandle;

	// 闪现/闪避技能：在编辑器里指定 GA 类即可
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA")
	TSubclassOf<UGameplayAbility> GADodgeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GA")
	FGameplayAbilitySpecHandle GADodgeHandle;
public:
	virtual void Tick(float DeltaTime) override;
	//捡起武器相关
	void PickUpWeapon(AWeaponBase* NewWeapon);
	//捡起剑相关
	void PickUpSword(ASwordBase* NewSword);
	//状态相关
	UFUNCTION(BlueprintCallable, Category = "Hero")
	void SetCurrentHeroState(EHeroState NewState);
	UFUNCTION(BlueprintCallable, Category = "Hero")
	FORCEINLINE EHeroState GetCurrentHeroState() { return CurrentHeroState; }

	// 仅空手/持剑生效：切换为朝控制器或朝运动方向
	UFUNCTION(BlueprintCallable, Category = "Hero|Camera")
	void ToggleFacingMode();

	// 查询当前实际朝向模式（持枪时始终返回 FaceController）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hero|Camera")
	EHeroFacingMode GetCurrentFacingMode() const;

	// 按移动方向相对视角判断：前/后/左/右（用于闪避）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hero|Movement")
	EHeroMoveDirection GetMoveDirectionByView() const;
	
	//开火相关
	// 是否正在开火（已整合到GA，不再经过武器判断）
	UPROPERTY(BlueprintReadWrite, Category = "Hero")
	bool bIsFiring = false;
	
	void Fire();
	void StopFire();
	void Reload();
	void Melee();
	void Combo();
	void Dodge();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")	
	void AimFocusOpen();
	UFUNCTION(BlueprintImplementableEvent,Category = "Hero")
	void AimFocusClose();
	
	//获取当前武器
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Hero")
	FORCEINLINE AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }

	//获取当前剑
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hero")
	FORCEINLINE ASwordBase* GetCurrentSword() { return CurrentSword; }
    	
	
	//后坐力（底层偏移接口，保留供蓝图调用）
	UFUNCTION(BlueprintCallable, Category = "Offset")
	void AddPitchOffset(float PitchOffsetAmount);
	UFUNCTION(BlueprintCallable, Category = "Offset")
	void AddYawOffset(float YawOffsetAmount);

	/**
	 * GA调用：施加一次后坐力
	 * @param HorizontalRecoil  水平后坐力基础值（如 0.1）
	 * @param VerticalRecoil    垂直后坐力基础值（如 0.7）
	 * @param MaxRecoilCount    超过此连发数后垂直后坐力大幅衰减（水平随机漂移增强）
	 */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ApplyRecoil(float HorizontalRecoil, float VerticalRecoil, int32 MaxRecoilCount);

	/** 重置后坐力计数（停止开火时调用） */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ResetRecoil();
	
	//warp
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateWarpTarget();

	// 尝试授予 Dodge 技能（ASC 就绪后调用）
	void GrantDodgeAbility();

	// 按当前状态应用朝向策略
	void ApplyFacingModeByState();

protected:
	// ---- 后坐力内部状态 ----

	/** 当前连发计数（float方便逐帧平滑恢复） */
	float RecoilShotCount = 0.f;

	/** 待施加的 Pitch 后坐力余量（每帧通过插值逐步消耗施加） */
	float PendingRecoilPitch = 0.f;

	/** 待施加的 Yaw 后坐力余量 */
	float PendingRecoilYaw = 0.f;

	/** 每次后坐力在多少秒内施加完毕（越小越干脆，建议 0.05~0.15） */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilApplyTime = 0.03f;

	/** 当前后坐力剩余时间 */
	float RecoilPitchTimeRemaining = 0.f;
	float RecoilYawTimeRemaining = 0.f;

	/** 上次开枪后经过的时间，用于判断何时开始恢复后坐力计数 */
	float TimeSinceLastShot = 0.f;

	/** 停止射击多少秒后后坐力计数开始恢复（建议 0.3~0.8） */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoveryDelay = 0.3f;

	/** 每秒恢复多少"发"的后坐力计数（建议 3~8，值越大恢复越快） */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoverySpeed = 8.f;

	/** 速度低于该值时视为静止，返回 None */
	UPROPERTY(EditDefaultsOnly, Category = "Hero|Movement")
	float MoveDirectionMinSpeed = 5.f;

	// 空手/持剑时是否朝控制器方向；false 为朝运动方向（默认）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero|Camera")
	bool bControllerFacingModeForMelee = false;
	

	
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
};
