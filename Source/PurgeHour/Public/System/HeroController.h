// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "HeroController.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHeroController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputMappingContext* HeroIMC;
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputAction* HeroMove;
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputAction* HeroLook;
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputAction* HeroJump;
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputAction* HeroFire;
	UPROPERTY(EditDefaultsOnly,Category="Input")
	UInputAction* HeroFocusAim;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void Move(const FInputActionValue& Value) ;
	void Look(const FInputActionValue& Value);
	void Jump() ;
	void JumpEnd() ;
	void Fire() ;
	void StopFire () ;
	void FocusAimOpen () ;
	void FocusAimClose () ;
	
};
