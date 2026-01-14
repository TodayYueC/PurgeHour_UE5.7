// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/Hero.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/WeaponBase.h"

AHero::AHero()
{
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
	}
}

void AHero::Fire()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon && CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}

void AHero::StopFire()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon && CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}



