// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "HeroPlayerState.generated.h"

class UHeroYueASC;
class UHeroAttributeSet;

/**
 * 
 */
UCLASS()
class PURGEHOUR_API AHeroPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AHeroPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UHeroAttributeSet* GetAttributeSet() const{ return AttributeSet;}
	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UHeroAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UHeroYueASC> AbilitySystemComponent;
	
};
