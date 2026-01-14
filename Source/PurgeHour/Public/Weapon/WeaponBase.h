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
	// 拾取者
	UPROPERTY(BlueprintReadOnly,Category = "Weapon")
	TObjectPtr<AHero> WeaponOwner;

	
	UFUNCTION()
	void OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	FTimerHandle ShootTimerHandle;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable,Category="Weapon")
	FORCEINLINE UWeaponData* GetWeaponDataAsset() const { return WeaponDataAsset; }
	
	void Fire();
	void StopFire();
	void ShootTime();
};
