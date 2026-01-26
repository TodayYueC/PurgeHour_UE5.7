// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FightWidget.h"
#include "Components/TextBlock.h"
#include "Subsystem/DelegatesSubsystem.h"

void UFightWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UDelegatesSubsystem* DelegatesSubsystem = GetGameInstance()->GetSubsystem<UDelegatesSubsystem>();
	if (DelegatesSubsystem)
	{
		DelegatesSubsystem->WeaponNameChangedDelegate.AddUObject(this, &UFightWidget::UpdateWeaponName);
		DelegatesSubsystem->CurrentBulletNumChangedDelegate.AddUObject(this, &UFightWidget::UpdateCurrentBullets);
		DelegatesSubsystem->AllBulletNumChangedDelegate.AddUObject(this, &UFightWidget::UpdateAllBullets);
	}
}

void UFightWidget::UpdateWeaponName(const FText& NewWeaponName)
{
	//if (this->WeaponName)
	//{
		//this->
		WeaponName->SetText(NewWeaponName);
	//}
}

void UFightWidget::UpdateCurrentBullets(int32 NewBulletNum)
{
	CurrentBullets->SetText(FText::AsNumber(NewBulletNum));
}

void UFightWidget::UpdateAllBullets(int32 NewAllBulletNum)
{
	AllBullets->SetText(FText::AsNumber(NewAllBulletNum));
}

//FText::FromString(WeaponName)
