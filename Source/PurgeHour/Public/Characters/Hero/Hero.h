// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Hero.generated.h"
class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHero : public ACharacterBase
{
	GENERATED_BODY()
public:
	AHero();
	
protected:
	virtual void BeginPlay() override;
	
	
	//摄像机与弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	
public:
	virtual void Tick(float DeltaTime) override;
	
};
