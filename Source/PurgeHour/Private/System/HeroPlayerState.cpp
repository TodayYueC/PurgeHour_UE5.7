// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HeroPlayerState.h"
#include "AbilitySystem/HeroYueASC.h"
#include "AbilitySystem/HeroAttributeSet.h"
#include "AbilitySystemComponent.h"

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

void AHeroPlayerState::BeginPlay()
{
	Super::BeginPlay();
	BindAttributeCallbacks();
}

void AHeroPlayerState::BindAttributeCallbacks()
{
	if (!AbilitySystemComponent) return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UHeroAttributeSet::GetHealthAttribute()).AddUObject(this, &AHeroPlayerState::OnHealthAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UHeroAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &AHeroPlayerState::OnMaxHealthAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UHeroAttributeSet::GetAmmoAttribute()).AddUObject(this, &AHeroPlayerState::OnAmmoAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UHeroAttributeSet::GetMaxAmmoAttribute()).AddUObject(this, &AHeroPlayerState::OnMaxAmmoAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UHeroAttributeSet::GetReserveAmmoAttribute()).AddUObject(this, &AHeroPlayerState::OnReserveAmmoAttributeChanged);
}

void AHeroPlayerState::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void AHeroPlayerState::OnMaxHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void AHeroPlayerState::OnAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAmmoChanged.Broadcast(static_cast<int32>(Data.NewValue));
}

void AHeroPlayerState::OnMaxAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnMaxAmmoChanged.Broadcast(static_cast<int32>(Data.NewValue));
}

void AHeroPlayerState::OnReserveAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnReserveAmmoChanged.Broadcast(static_cast<int32>(Data.NewValue));
}

void AHeroPlayerState::BroadcastWeaponName(const FText& WeaponName)
{
	OnWeaponNameChanged.Broadcast(WeaponName);
}

void AHeroPlayerState::BroadcastAmmo()
{
	if (!AttributeSet) return;
	OnAmmoChanged.Broadcast(static_cast<int32>(AttributeSet->GetAmmo()));
	OnReserveAmmoChanged.Broadcast(static_cast<int32>(AttributeSet->GetReserveAmmo()));
}

