// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/Hero.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponBase.h"
#include "AbilitySystem/HeroYueASC.h"
#include "System/HeroPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/GA/BaseWeaponGA.h"

AHero::AHero()
{
	// Hero 需要 Tick（CharacterBase 已关闭，此处单独开启）
	PrimaryActorTick.bCanEverTick = true;
	
	//////////////////////////////摄像机与弹簧臂组件//////////////////////////////////
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 250.0f; // 弹簧臂长度
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComponent->SocketOffset = FVector(0.0f, 65.0f, 40.0f); // 偏移位置
	SpringArmComponent->bUsePawnControlRotation = true; // 随控制器旋转，处理视角上下移动
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // 摄像机不随控制器旋转
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	//////////////////////////////操作手感优化//////////////////////////////////
	JumpMaxHoldTime = 0.2f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AHero::BeginPlay()
{
	Super::BeginPlay();
	CurrentHeroState = EHeroState::EmptyHanded;
	Init();
}

void AHero::Init()
{
	InitGEToSelf();
}

void AHero::InitGEToSelf()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent || !InitGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System Component Or InitGE is NULL"));
		return;
	}
	
	FGameplayEffectContextHandle InitContextHandle = AbilitySystemComponent->MakeEffectContext();
	InitContextHandle.AddInstigator(this,this);
	
	FGameplayEffectSpecHandle InitSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitGE, 1, InitContextHandle);
	if (InitSpecHandle.IsValid())
	{		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitSpecHandle.Data.Get());
	}
}

void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHero::PickUpWeapon(AWeaponBase* NewWeapon)
{
	if (NewWeapon)
	{
		//先直接捡，以后再扩展背包系统
		CurrentWeapon = NewWeapon;
		SetCurrentHeroState(EHeroState::HoldingWeapon);
		// 禁用碰撞
		NewWeapon->SetActorEnableCollision(false);

		//持枪：强制吸附到插槽位置
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	
		//赋予枪械技能
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (ASC && CurrentWeapon->GetGAFireClass())
		{
			FGameplayAbilitySpec FireSpec(CurrentWeapon->GetGAFireClass(), 1, -1, CurrentWeapon);
			FireSpec.GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Fire")));
			GAFireHandle= ASC->GiveAbility(FireSpec);
		}
		if (ASC && CurrentWeapon->GetGAReloadClass())
		{
			FGameplayAbilitySpec ReloadSpec(CurrentWeapon->GetGAReloadClass(), 1, -1, CurrentWeapon);
			GAReloadHandle= ASC->GiveAbility(ReloadSpec);
		}
	}
}

void AHero::Fire()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon && CurrentWeapon)
	{
		// 开火已整合到GA，仅设置状态标记
		bIsFiring = true;
		
		// 以下调用武器开火的逻辑已整合到GA，注释保留
		// if (CurrentWeapon->GetCurrentBulletNum()<=0)
		// {
		// 	UGameplayStatics::PlaySound2D(GetWorld(), EmptyMagazineSound);
		// }
		// CurrentWeapon->Fire();
		if (GetAbilitySystemComponent() && GAFireHandle.IsValid())
		{
			FGameplayAbilitySpec* Spec = GetAbilitySystemComponent()->FindAbilitySpecFromHandle(GAFireHandle);
			if (Spec)
			{
				Spec->InputPressed = true;
			}
			GetAbilitySystemComponent()->TryActivateAbility(GAFireHandle);
		}
	}
}

void AHero::Reload()
{
	if (GetAbilitySystemComponent() && GAReloadHandle.IsValid())
	{
		GetAbilitySystemComponent()->TryActivateAbility(GAReloadHandle);
	}
}

void AHero::StopFire()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon && CurrentWeapon)
	{
		// 开火已整合到GA，仅清除状态标记
		bIsFiring = false;
		if (GetAbilitySystemComponent() && GAFireHandle.IsValid())
		{
			FGameplayAbilitySpec* FireSpec = GetAbilitySystemComponent()->FindAbilitySpecFromHandle(GAFireHandle);
			if (FireSpec)
			{
				FireSpec->InputPressed = false; // 取消输入标记，触发GA中停止开火的逻辑
				GetAbilitySystemComponent()->CancelAbilityHandle(GAFireHandle); // 直接取消技能，确保开火停止
			}
		}
		
		
		// 以下调用武器停火的逻辑已整合到GA，注释保留
		// CurrentWeapon->StopFire();
	}
}

void AHero::AddPitchOffset(float PitchOffsetAmount)
{
	AddControllerPitchInput(PitchOffsetAmount);
}

void AHero::AddYawOffset(float YawOffsetAmount)
{
	AddControllerYawInput(YawOffsetAmount);
}

UAbilitySystemComponent* AHero::GetAbilitySystemComponent() const
{
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		return HeroPlayerState->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AHero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		HeroPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(HeroPlayerState, this);
	}
}

void AHero::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		HeroPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(HeroPlayerState, this);
	}
}



