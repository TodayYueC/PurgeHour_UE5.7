// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class AHero;
class UWeaponData;
class USphereComponent;
class UBaseWeaponGA;

UCLASS()
class PURGEHOUR_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 组件
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;
	
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	TObjectPtr<USphereComponent> PickupCollision;
	
	// 数据
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon")
	TObjectPtr<UWeaponData> WeaponDataAsset;
	
	// 子弹数量已迁移到GAS属性集，注释保留
	// int32 CurrentBulletNum;
	// int32 AllBulletNum;
	
	int32 OffsetNum;
	float PendingRecoilPitch = 0.0f;
	float PendingRecoilYaw = 0.0f;
	
	UPROPERTY(BlueprintReadOnly,Category="Offset")
	float PitchOffset;
	
	UPROPERTY(BlueprintReadOnly,Category="Offset")
	float YawOffset;
	
	// 拾取者
	UPROPERTY(BlueprintReadOnly,Category = "Weapon")
	TObjectPtr<AHero> WeaponOwner;
	
	//击中粒子特效
	UPROPERTY(EditDefaultsOnly,Category="ParticleSystem")
	TObjectPtr<UParticleSystem> DirtEffect;
	
	UPROPERTY(EditDefaultsOnly,Category="ParticleSystem")
	TObjectPtr<UParticleSystem> GrassEffect;
	
	UPROPERTY(EditDefaultsOnly,Category="ParticleSystem")
	TObjectPtr<UParticleSystem> RockEffect;
	
	UPROPERTY(EditDefaultsOnly,Category="ParticleSystem")
	TObjectPtr<UParticleSystem> MetalEffect;
	
	UPROPERTY(EditDefaultsOnly,Category="ParticleSystem")
	TObjectPtr<UParticleSystem> WoodEffect;
	
	//击中音效
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> DirtSound;
	
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> GrassSound;
	
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> RockSound;
	
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> MetalSound;
	
	UPROPERTY(EditDefaultsOnly,Category="Sound")
	TObjectPtr<USoundCue> WoodSound;
	
	//弹孔贴花
	UPROPERTY(EditDefaultsOnly,Category="Decal")
	TObjectPtr<UMaterialInterface> BulletDecalMaterial;
	


	UFUNCTION()
	void OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// 开火计时器已迁移到GAS，注释保留
	// FTimerHandle ShootTimerHandle;
	
	//初始化数据
	void InitWeaponData();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable,Category="Weapon")
	FORCEINLINE UWeaponData* GetWeaponDataAsset() const { return WeaponDataAsset; }
	
	// 子弹数量相关函数已迁移到GAS属性集，注释保留
	// UFUNCTION(BlueprintCallable,Category="Bullet")
	// FORCEINLINE int32 GetCurrentBulletNum() const { return CurrentBulletNum; }
	//
	// UFUNCTION(BlueprintCallable,Category="Bullet")
	// FORCEINLINE int32 GetAllBulletNum() const { return AllBulletNum; }
	
	// 以下开火相关函数已全部迁移到GAS，注释保留
	// void Fire();
	// void StopFire();
	// void ShootTime();
	
	// 子弹UI广播已迁移到GAS属性集，注释保留
	// void InitBulletNumUI();
	
	//获取拥有者
	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Weapon")
	FORCEINLINE AHero* GetWeaponOwner()  { return WeaponOwner; }
	
	//技能系统
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UBaseWeaponGA> GAFireClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GAS")	
	TSubclassOf<UBaseWeaponGA> GAReloadClass;
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	TSubclassOf<UBaseWeaponGA> GetGAFireClass() const { return GAFireClass; }
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	TSubclassOf<UBaseWeaponGA> GetGAReloadClass() const { return GAReloadClass; }

	// 获取静态网格体组件
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FORCEINLINE UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	// 获取骨骼网格体组件
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FORCEINLINE USkeletalMeshComponent* GetWeaponSkeletalMesh() const { return WeaponSkeletalMesh; }
	
	
};
