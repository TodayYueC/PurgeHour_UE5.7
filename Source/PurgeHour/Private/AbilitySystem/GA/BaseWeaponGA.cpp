// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GA/BaseWeaponGA.h"
#include "Characters/Hero/Hero.h"
#include "Weapon/WeaponBase.h"
UBaseWeaponGA::UBaseWeaponGA()
{
	
}

AWeaponBase* UBaseWeaponGA::GetCurrentWeapon()
{
	// 获取拥有者角色与武器
	AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo());
	if (Hero)
	{
		return Hero->GetCurrentWeapon();
	}
	
	return nullptr;
}
