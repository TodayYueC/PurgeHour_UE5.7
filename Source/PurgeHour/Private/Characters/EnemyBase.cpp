// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "AbilitySystem/EnemyASC.h"
#include "AbilitySystem/EnemyAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyBase::AEnemyBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<UEnemyASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	ApplyInitGE();

	// 授予死亡GA（不自动激活，由属性集血量归零时触发）
	if (AbilitySystemComponent && DeathGA)
	{
		FGameplayAbilitySpec DeathSpec(DeathGA, 1, -1, this);
		DeathGAHandle = AbilitySystemComponent->GiveAbility(DeathSpec);
	}
}

void AEnemyBase::ApplyInitGE()
{
	if (!AbilitySystemComponent || !InitGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::ApplyInitGE - ASC or InitGE is NULL on %s"), *GetName());
		return;
	}

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddInstigator(this, this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitGE, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AEnemyBase::TriggerDeathGA()
{
	if (AbilitySystemComponent && DeathGAHandle.IsValid())
	{
		AbilitySystemComponent->TryActivateAbility(DeathGAHandle);
	}
}

void AEnemyBase::SetDead()
{
	if (bIsDead) return; // 防止重复调用
	bIsDead = true;

	// 关闭胶囊体碰撞：角色不再被阻挡，Pawn之间也不再产生推挤
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 网格体也设为忽略，避免挡住射线/Overlap
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TriggerDeathGA();
}

