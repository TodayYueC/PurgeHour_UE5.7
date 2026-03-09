// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FightWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "System/HeroPlayerState.h"
#include "AbilitySystem/HeroAttributeSet.h"
#include "GameFramework/PlayerController.h"

void UFightWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AHeroPlayerState* PS = PC->GetPlayerState<AHeroPlayerState>())
		{
			PS->OnWeaponNameChanged.AddDynamic(this, &UFightWidget::UpdateWeaponName);
			 PS->OnAmmoChanged.AddDynamic(this, &UFightWidget::UpdateCurrentBullets);
			 PS->OnReserveAmmoChanged.AddDynamic(this, &UFightWidget::UpdateReserveAmmo);
			PS->OnHealthChanged.AddDynamic(this, &UFightWidget::UpdateHealth);
			PS->OnMaxHealthChanged.AddDynamic(this, &UFightWidget::UpdateMaxHealth);

			// 开局立即用 AttributeSet 当前值刷新一次血量和弹药 UI
			if (const UHeroAttributeSet* AS = PS->GetAttributeSet())
			{
				UpdateMaxHealth(AS->GetMaxHealth());
				UpdateHealth(AS->GetHealth());
				// UpdateCurrentBullets(static_cast<int32>(AS->GetAmmo()));
				// UpdateReserveAmmo(static_cast<int32>(AS->GetReserveAmmo()));
			}
		}
	}
}

void UFightWidget::UpdateWeaponName(const FText& NewWeaponName)
{
	WeaponName->SetText(NewWeaponName);
}

void UFightWidget::UpdateCurrentBullets(int32 NewBulletNum)
{
	CurrentBullets->SetText(FText::AsNumber(NewBulletNum));
}

void UFightWidget::UpdateReserveAmmo(int32 NewReserveAmmo)
{
	AllBullets->SetText(FText::AsNumber(NewReserveAmmo));
}

void UFightWidget::UpdateHealth(float NewHealth)
{
	if (HealthBar && MaxHealth > 0.f)
	{
		HealthBar->SetPercent(NewHealth / MaxHealth);
	}
}

void UFightWidget::UpdateMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	// MaxHealth 变化时同步刷新血条显示（需要知道当前 Health）
	// 实际 Health 会在下一次 OnHealthChanged 时更新，此处先不额外查询
}
