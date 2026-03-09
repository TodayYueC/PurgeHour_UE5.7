// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GA/BaseWeaponGA.h"
#include "Characters/Hero/Hero.h"
#include "Weapon/WeaponBase.h"
#include "Data/WeaponData.h"
#include "System/HeroPlayerState.h"
#include "AbilitySystem/HeroAttributeSet.h"

UBaseWeaponGA::UBaseWeaponGA()
{
	
}

AWeaponBase* UBaseWeaponGA::GetCurrentWeapon()
{
	AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo());
	if (Hero)
	{
		return Hero->GetCurrentWeapon();
	}
	return nullptr;
}

void UBaseWeaponGA::CalculateReload()
{
	AWeaponBase* Weapon = GetCurrentWeapon();
	if (!Weapon || !Weapon->GetWeaponDataAsset()) return;

	AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo());
	if (!Hero) return;

	AHeroPlayerState* PS = Hero->GetPlayerState<AHeroPlayerState>();
	if (!PS || !PS->GetAttributeSet()) return;

	const UHeroAttributeSet* AS = PS->GetAttributeSet();

	// 弹夹容量上限
	const int32 MagazineSize   = Weapon->GetWeaponDataAsset()->MaxWeaponBulletNum;
	// 当前弹夹内子弹
	const int32 CurrentAmmo    = static_cast<int32>(AS->GetAmmo());
	// 当前备弹
	const int32 ReserveAmmo    = static_cast<int32>(AS->GetReserveAmmo());

	// 需要从备弹补充的数量
	const int32 AmmoNeeded     = MagazineSize - CurrentAmmo;
	// 实际能补充的数量（备弹不足时取全部备弹）
	const int32 AmmoToTransfer = FMath::Min(AmmoNeeded, ReserveAmmo);

	const int32 NewCurrentAmmo = CurrentAmmo + AmmoToTransfer;
	const int32 NewReserveAmmo = ReserveAmmo - AmmoToTransfer;

	// 应用 GE
	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle EffectSpec = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadGEClass, 1.0f, EffectContext);

	if (EffectSpec.IsValid())
	{
		EffectSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Weapon.Ammo")), NewCurrentAmmo);
		EffectSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Weapon.ReserveAmmo")), NewReserveAmmo);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	}
}

