// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GA/BaseSwordGA.h"
#include "Weapon/SwordBase.h"
#include "Characters/Hero/Hero.h"

ASwordBase* UBaseSwordGA::GetCurrentWeapon()
{
	AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo());
	if (Hero)
	{
		return Hero->GetCurrentSword();
	}
	
	return nullptr;
}
