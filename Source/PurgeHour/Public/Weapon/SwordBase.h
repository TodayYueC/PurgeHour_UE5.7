// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwordBase.generated.h"

class AHero;
class USwordData;
class USphereComponent;
class UBaseSwordGA;

UCLASS()
class PURGEHOUR_API ASwordBase : public AActor
{
	GENERATED_BODY()

public:
	ASwordBase();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// ---- 组件 ----
	UPROPERTY(VisibleAnywhere, Category = "Sword")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Sword")
	TObjectPtr<UStaticMeshComponent> SwordMesh;

	UPROPERTY(VisibleAnywhere, Category = "Sword")
	TObjectPtr<USphereComponent> PickupCollision;

	// ---- 数据 ----
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword")
	TObjectPtr<USwordData> SwordDataAsset;

	// ---- 拾取者 ----
	UPROPERTY(BlueprintReadOnly, Category = "Sword")
	TObjectPtr<AHero> SwordOwner;

	// ---- 内部 ----

	UFUNCTION()
	void OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

public:
	// ---- 蓝图可调用函数 ----

	/** 获取剑的DataAsset */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sword")
	FORCEINLINE USwordData* GetSwordDataAsset() const { return SwordDataAsset; }

	/** 获取拾取者 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sword")
	FORCEINLINE AHero* GetSwordOwner() const { return SwordOwner; }

	/** 获取静态网格体组件 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sword")
	FORCEINLINE UStaticMeshComponent* GetSwordMesh() const { return SwordMesh; }
	
	//GAS
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UBaseSwordGA> GAAttackClass;
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	TSubclassOf<UBaseSwordGA> GetGAAttackClass() const { return GAAttackClass; }
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UBaseSwordGA> GAComboClass;
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	TSubclassOf<UBaseSwordGA> GetGAComboClass() const { return GAComboClass; }
};
