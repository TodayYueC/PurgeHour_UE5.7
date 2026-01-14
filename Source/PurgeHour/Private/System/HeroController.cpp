// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HeroController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Hero/Hero.h"

void AHeroController::BeginPlay()
{
	Super::BeginPlay();
	////////////////////////////////////激活IMC///////////////////////////////////
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (HeroIMC){
			Subsystem->AddMappingContext(HeroIMC, 0);
		}
	}
}

void AHeroController::SetupInputComponent()
{
	Super::SetupInputComponent();
	////////////////////////////////////绑定输入///////////////////////////////////
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (HeroMove)
			EnhancedInputComponent->BindAction(HeroMove, ETriggerEvent::Triggered, this, &AHeroController::Move);
		if (HeroLook)
			EnhancedInputComponent->BindAction(HeroLook, ETriggerEvent::Triggered, this, &AHeroController::Look);
		if (HeroJump){
			EnhancedInputComponent->BindAction(HeroJump, ETriggerEvent::Started, this, &AHeroController::Jump);
			EnhancedInputComponent->BindAction(HeroJump, ETriggerEvent::Completed, this, &AHeroController::JumpEnd);
		}
		if (HeroFire)
		{
			EnhancedInputComponent->BindAction(HeroFire, ETriggerEvent::Started, this, &AHeroController::Fire);
			EnhancedInputComponent->BindAction(HeroFire, ETriggerEvent::Completed, this, &AHeroController::StopFire);
		}
		if (HeroFocusAim)
		{
			EnhancedInputComponent->BindAction(HeroFocusAim, ETriggerEvent::Started, this, &AHeroController::FocusAimOpen);
			EnhancedInputComponent->BindAction(HeroFocusAim, ETriggerEvent::Completed, this, &AHeroController::FocusAimClose);
		}
	}
}

void AHeroController::Move (const FInputActionValue& Value) 
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	if (ACharacter* ControlledPawn = GetCharacter())
	{
		FRotator MoveRotation(0.0f,GetControlRotation().Yaw,0.0f);
		FVector Forward = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(MoveRotation).GetUnitAxis(EAxis::Y);
		ControlledPawn->AddMovementInput(Forward,MoveVector.Y);
		ControlledPawn->AddMovementInput(Right,MoveVector.X);
	}
}

void AHeroController::Look(const FInputActionValue& Value) 
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (GetCharacter()){
		AddYawInput(LookVector.X);
		AddPitchInput(LookVector.Y);
	}
}

void AHeroController::Jump() 
{
	if (GetCharacter())
		GetCharacter()->Jump();
	
}

void AHeroController::JumpEnd() 
{
	if (GetCharacter())
		GetCharacter()->StopJumping();
}

void AHeroController::Fire()
{
	AHero* HeroCharacter = GetPawn<AHero>();
	if (IsValid(HeroCharacter))
	{
		HeroCharacter->Fire();
	}
}

void AHeroController::StopFire()
{
	AHero* HeroCharacter = GetPawn<AHero>();
	if (IsValid(HeroCharacter))
	{
		HeroCharacter->StopFire();
	}
}


void AHeroController::FocusAimOpen()
{
	AHero* HeroCharacter = GetPawn<AHero>();
	if (IsValid(HeroCharacter) && HeroCharacter->GetCurrentHeroState() == EHeroState::HoldingWeapon)
	{
		HeroCharacter->AimFocusOpen();
	}
}

void AHeroController::FocusAimClose()
{
	AHero* HeroCharacter = GetPawn<AHero>();
	if (IsValid(HeroCharacter) && HeroCharacter->GetCurrentHeroState() == EHeroState::HoldingWeapon)
	{
		HeroCharacter->AimFocusClose();
	}
}
