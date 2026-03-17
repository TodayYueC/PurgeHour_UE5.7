// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "GameplayAbilitySpec.h"
#include "EnemyBase1.generated.h"

class UBoxComponent;
class USphereComponent;
class UGameplayAbility;

UCLASS()
class PURGEHOUR_API AEnemyBase1 : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeToChase")
	TObjectPtr<USphereComponent> RangeToChase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UBoxComponent> AttackCollision;
	
	UPROPERTY(EditAnywhere,Category="MontageAttack")
	TObjectPtr<UAnimMontage> MontageAttack;

	// Attack ability granted on BeginPlay and activated from Attack().
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GA")
	TSubclassOf<UGameplayAbility> AttackGA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GA")
	FGameplayAbilitySpecHandle AttackGAHandle;
	
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> HitVictims;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable,Category="Attack")
	void Attack();
	
	UFUNCTION(BlueprintCallable,Category="Attack")
	void SetCollisionActive(bool bIsActive);
	
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

};
