// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HeroPlayerState.h"
#include "AbilitySystem/HeroYueASC.h"
#include "AbilitySystem/HeroAttributeSet.h"

AHeroPlayerState::AHeroPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHeroYueASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	
	
	AttributeSet = CreateDefaultSubobject<UHeroAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AHeroPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
