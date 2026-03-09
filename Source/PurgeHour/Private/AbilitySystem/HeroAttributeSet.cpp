// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HeroAttributeSet.h"
#include "GameplayEffectExtension.h"

UHeroAttributeSet::UHeroAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitAmmo(30.f);
	InitMaxAmmo(30.f);
	InitReserveAmmo(90.f); // 备弹初始值
}

void UHeroAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetAmmoAttribute())
	{
		// Clamp 弹夹子弹：[0, MaxAmmo（弹夹上限）]
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmo());
	}

	if (Attribute == GetReserveAmmoAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		if (GetHealth() <= 0.f)
		{
			// 角色死亡逻辑
		}
	}
	

	// 弹药/血量 UI 广播已迁移到 HeroPlayerState 属性回调，此处不再需要
}
