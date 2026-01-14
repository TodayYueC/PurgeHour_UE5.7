// Fill out your copyright notice in the Description page of Project Settings.


#include "System/FightHUD.h"
#include "UI/FightWidget.h"

void AFightHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (FightWidgetClass)
	{
		FightWidget = CreateWidget<UFightWidget>(GetWorld(), FightWidgetClass);
		
	}
	if (FightWidget)
	{
		FightWidget->AddToViewport();
	}
}
