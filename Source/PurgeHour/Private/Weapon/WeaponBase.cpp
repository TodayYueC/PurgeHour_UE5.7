// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Data/WeaponData.h"
#include "Characters/Hero/Hero.h"
#include "Engine/Engine.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->SetupAttachment(Root);
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(Root);
	
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnPickupBeginOverlap);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp )
	{
		WeaponOwner = Cast<AHero>(OtherActor);
		if (WeaponOwner){
			//先直接捡，以后改进背包系统
			WeaponOwner->PickUpWeapon(this);
			WeaponMesh->DestroyComponent();
			PickupCollision->DestroyComponent();
		}
	}
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (WeaponDataAsset)
	{
		WeaponSkeletalMesh->SetSkeletalMesh(WeaponDataAsset->SkeletalMesh);
		WeaponMesh->SetStaticMesh(WeaponDataAsset->StaticMesh);
	}
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Fire()
{
	if (WeaponDataAsset)
	{
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWeaponBase::ShootTime, WeaponDataAsset->AttackSpeed, true, 0.0f);
	}
}

void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
}

void AWeaponBase::ShootTime()
{
	//先用debug代替，以后写逻辑
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Shoot") );
}

