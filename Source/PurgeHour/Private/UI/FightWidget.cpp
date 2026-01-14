// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FightWidget.h"
#include "Components/TextBlock.h"
#include "Subsystem/DelegatesSubsystem.h"

void UFightWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->WeaponNameChangedDelegate.BindUObject(this, &UFightWidget::UpdateWeaponName);
}

void UFightWidget::UpdateWeaponName(const FText& NewWeaponName)
{
	if (this->WeaponName)
	{
		this->WeaponName->SetText(NewWeaponName);
	}
}
//FText::FromString(WeaponName)