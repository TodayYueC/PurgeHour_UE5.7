// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/EnemyAttributeSet.h"
#include "Characters/EnemyBase.h"
#include "GameplayEffectExtension.h"

UEnemyAttributeSet::UEnemyAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
}

void UEnemyAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		if (GetHealth() <= 0.f)
		{
			// 通过SetDead统一处理：关碰撞 + 激活死亡GA
			if (AEnemyBase* EnemyOwner = Cast<AEnemyBase>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
			{
				EnemyOwner->SetDead();
			}
		}
	}
}
