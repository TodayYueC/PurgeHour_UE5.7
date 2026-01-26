// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ACharacterBase::UniDoLineTrace(const FVector& StartLocation,const FRotator& StartRotation,float TraceDistance,FHitResult& HitResult,bool bDraw)
{
	//射线检测
	FVector EndLocation = StartLocation + StartRotation.Vector() * TraceDistance;
	FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
	
	//Debug用
	if (bDraw)
	{
		if (bHit)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Green, false, 1.0f, 0, 0.08f);
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.0f, 12, FColor::Red, false, 1.0f);
		}
		else
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, false, 1.0f, 0, 0.08f);
		}
	}
	return bHit;
}

