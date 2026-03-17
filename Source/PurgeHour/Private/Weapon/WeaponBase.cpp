// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"

#include "Components/SphereComponent.h"
// #include "Components/DecalComponent.h"  // 已迁移到GAS
#include "Data/WeaponData.h"
#include "Characters/Hero/Hero.h"
// #include "Engine/Engine.h"  // 已无使用
// #include "Kismet/GameplayStatics.h"     // 已迁移到GAS
// #include "Sound/SoundCue.h"             // 已迁移到GAS
// #include "Subsystem/DelegatesSubsystem.h" // 武器名广播已迁移到 PlayerState

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->SetupAttachment(Root);
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(Root);
	PickupCollision->SetSphereRadius(100.f);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupCollision->SetGenerateOverlapEvents(true);
	
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnPickupBeginOverlap);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// 防止蓝图子类或关卡实例覆盖掉碰撞默认值，运行时再强制一次。
	if (PickupCollision)
	{
		PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
		PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		PickupCollision->SetGenerateOverlapEvents(true);
	}

	// InitWeaponData(); // 旧初始化逻辑已全部迁移，注释保留
}

void AWeaponBase::OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp )
	{
		WeaponOwner = Cast<AHero>(OtherActor);
		if (WeaponOwner){
			// 仅负责拾取，UI 由 Hero 根据当前手持状态统一刷新
			WeaponOwner->PickUpWeapon(this);
			// 子弹UI已迁移到GAS属性集广播，注释保留
			// InitBulletNumUI();
			
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

void AWeaponBase::InitWeaponData()
{
	// 旧后坐力赋值已迁移到 Hero::ApplyRecoil，注释保留
	// if (WeaponDataAsset)
	// {
	// 	PitchOffset = WeaponDataAsset->RecoilPitchOffset;
	// 	YawOffset = WeaponDataAsset->RecoilYawOffset;
	// 	OffsetNum = 0;
	// }
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 后坐力已迁移到GAS，注释保留
	// if (WeaponOwner && (FMath::Abs(PendingRecoilPitch) > 0.01f || FMath::Abs(PendingRecoilYaw) > 0.01f))
	// {
	// 	float InterpPitch = FMath::FInterpTo(0.0f, PendingRecoilPitch, DeltaTime, 15.0f);
	// 	float InterpYaw = FMath::FInterpTo(0.0f, PendingRecoilYaw, DeltaTime, 15.0f);
	// 	WeaponOwner->AddPitchOffset(InterpPitch);
	// 	WeaponOwner->AddYawOffset(InterpYaw);
	// 	PendingRecoilPitch -= InterpPitch;
	// 	PendingRecoilYaw -= InterpYaw;
	// }
}

// 以下开火相关函数已全部迁移到GAS，注释保留
// void AWeaponBase::Fire()
// {
// 	if (WeaponDataAsset)
// 	{
// 		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWeaponBase::ShootTime, WeaponDataAsset->AttackSpeed, true, 0.0f);
// 	}
// }
//
// void AWeaponBase::StopFire()
// {
// 	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
// 	OffsetNum = 0;
// }
//
// void AWeaponBase::ShootTime()
// {
// 	if (CurrentBulletNum > 0)
// 	{
// 		CurrentBulletNum--;
// 		InitBulletNumUI();
// 	}
// 	else
// 	{
// 		WeaponOwner->StopFire();
// 		return;
// 	}
//
// 	// 角色开火动画已迁移到GA，此处注释
// 	// if (WeaponOwner && WeaponOwner->FireMontage)
// 	// {
// 	// 	WeaponOwner->PlayAnimMontage(WeaponOwner->FireMontage, 1.0f);
// 	// }
//
// 	if (WeaponOwner && WeaponOwner->GetController())
// 	{
// 		FVector StartLocation;
// 		FRotator StartRotation;
// 		WeaponOwner->GetController()->GetPlayerViewPoint(StartLocation, StartRotation);
// 		FHitResult HitResult;
//
// 		// 添加后坐力
// 		if (OffsetNum < 4)
// 		{
// 			OffsetNum++;
// 			WeaponOwner->AddPitchOffset(-PitchOffset);
// 			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset, YawOffset));
// 		}
// 		else if(OffsetNum < WeaponDataAsset->RecoilMaxCount)
// 		{
// 			OffsetNum++;
// 			WeaponOwner->AddPitchOffset(-PitchOffset*FMath::RandRange(2, OffsetNum));
// 			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
// 		}
// 		else
// 		{
// 			WeaponOwner->AddPitchOffset(-PitchOffset*2.0f);
// 			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
// 		}
//
// 		// 击中特效、音效、弹孔已迁移到GC，此处注释
// 		// if (bHit) { ... }
//
// 		// 开火动画与音效已迁移到GC，此处注释
// 		// if (WeaponDataAsset) { ... }
// 	}
// }

// 子弹UI广播已迁移到GAS属性集，注释保留
// void AWeaponBase::InitBulletNumUI()
// {
// 	GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->CurrentBulletNumChangedDelegate.Broadcast(CurrentBulletNum);
// 	GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->AllBulletNumChangedDelegate.Broadcast(AllBulletNum);
// }
