// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "EnemyBase.generated.h"

class UEnemyASC;
class UEnemyAttributeSet;
class UGameplayEffect;

/**
 * 敌人基类，挂载ASC与属性集，支持GAS
 */
UCLASS()
class PURGEHOUR_API AEnemyBase : public ACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GAS")
	UEnemyAttributeSet* GetEnemyAttributeSet() const { return AttributeSet; }

	/** 由AttributeSet血量归零时调用，激活死亡GA */
	void TriggerDeathGA();

	/** 标记怪物死亡：设置死亡状态、关闭碰撞，然后激活死亡GA */
	void SetDead();

	/** 是否已死亡（供蓝图/UpdateWarpTarget查询） */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UEnemyASC> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UEnemyAttributeSet> AttributeSet;

	/** 在蓝图中指定：用于初始化属性的GameplayEffect（Instant类型） */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitGE;

	/** 死亡时激活的GA（在蓝图子类中指定） */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> DeathGA;

	FGameplayAbilitySpecHandle DeathGAHandle;

	/** 是否已死亡 */
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bIsDead = false;

private:
	void ApplyInitGE();
};
