// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HeroAttributeSet.h"

UHeroAttributeSet::UHeroAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
}

void UHeroAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}
