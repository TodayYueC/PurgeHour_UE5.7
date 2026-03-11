// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SwordData.generated.h"

/**
 * 
 */
UCLASS()
class PURGEHOUR_API USwordData : public UDataAsset
{
	GENERATED_BODY()
public:
	//武器名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	FName SwordName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	FText SwordDisplayName;
	//武器静态网格体
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TObjectPtr<UStaticMesh> StaticMesh;
	
	//武器实体类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TSubclassOf<AActor> SwordActorClass;
	
	//伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	float Damage;
	
	
	//挥剑动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TObjectPtr<UAnimMontage> MeleeMontage1;
	
	//挥剑动画2
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TObjectPtr<UAnimMontage> MeleeMontage2;
	
	//挥剑动画3
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TObjectPtr<UAnimMontage> MeleeMontage3;
	
	//技能1动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TObjectPtr<UAnimMontage> SkillsMontage1;
	
	//Combo1
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	TArray<TObjectPtr<UAnimMontage>> Combo1;
	
	// //Combo2
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	// TArray<TObjectPtr<UAnimMontage>> Combo2;
	
	//武器描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sword")
	FText SwordDescription;
};
