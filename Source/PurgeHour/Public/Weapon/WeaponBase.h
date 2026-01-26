// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class AHero;
class UWeaponData;
class USphereComponent;

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
	UPROPERTY(EditDefaultsOnly,Category = "Weapon")
	TObjectPtr<UWeaponData> WeaponDataAsset;
	
	int32 CurrentBulletNum;
	
	int32 AllBulletNum;
	
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
	
	FTimerHandle ShootTimerHandle;
	
	//初始化数据
	void InitWeaponData();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable,Category="Weapon")
	FORCEINLINE UWeaponData* GetWeaponDataAsset() const { return WeaponDataAsset; }
	
	UFUNCTION(BlueprintCallable,Category="Bullet")
	FORCEINLINE int32 GetCurrentBulletNum() const { return CurrentBulletNum; }
	
	UFUNCTION(BlueprintCallable,Category="Bullet")
	FORCEINLINE int32 GetAllBulletNum() const { return AllBulletNum; }
	
	void Fire();
	void StopFire();
	void ShootTime();
	
	void InitBulletNumUI();
};
