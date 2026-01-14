// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class PURGEHOUR_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	//武器名称
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	FName WeaponName;
	//武器静态网格体
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<UStaticMesh> StaticMesh;
	//武器骨骼网格体
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	//武器实体类
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TSubclassOf<AActor> WeaponActorClass;
	//开火速度
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float AttackSpeed;
	//伤害
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float Damage;
	//子弹初始数量
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	int32 InitBulletNum;
	//弹夹最大数量
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	int32 MaxWeaponBulletNum;
	//开火音效
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<USoundCue> FireSound;
	//开火动画
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<UAnimationAsset> FireAnimation;
	//弹壳特效
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<UParticleSystem> BulletEffect;
	//武器缩略图
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TObjectPtr<UTexture2D> WeaponPic;
	//后坐力垂直偏移
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float RecoilPitchOffset;
	//后坐力水平偏移
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float RecoilYawOffset;
	//武器描述
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	FText WeaponDescription;
	
};
