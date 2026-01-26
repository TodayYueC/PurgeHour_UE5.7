// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Data/WeaponData.h"
#include "Characters/Hero/Hero.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Subsystem/DelegatesSubsystem.h"

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
	InitWeaponData();
	
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

			// 触发UI更新
			GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->WeaponNameChangedDelegate.Broadcast(WeaponDataAsset->WeaponDisplayName);
			InitBulletNumUI();
			
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
	if (WeaponDataAsset)
	{
		CurrentBulletNum = WeaponDataAsset->MaxWeaponBulletNum;
		AllBulletNum = 90; //先写死，以后捡子弹再改
		
		PitchOffset = WeaponDataAsset->RecoilPitchOffset;
		YawOffset = WeaponDataAsset->RecoilYawOffset;
		
		OffsetNum = 0;
		
	}
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (WeaponOwner && (FMath::Abs(PendingRecoilPitch) > 0.01f || FMath::Abs(PendingRecoilYaw) > 0.01f))
	{
		// 使用 FInterpTo 计算本帧应该移动的增量，15.0f 是平滑速度，值越大越快
		float InterpPitch = FMath::FInterpTo(0.0f, PendingRecoilPitch, DeltaTime, 15.0f);
		float InterpYaw = FMath::FInterpTo(0.0f, PendingRecoilYaw, DeltaTime, 15.0f);

		// 应用到 Hero
		WeaponOwner->AddPitchOffset(InterpPitch);
		WeaponOwner->AddYawOffset(InterpYaw);

		// 减去已经应用掉的部分
		PendingRecoilPitch -= InterpPitch;
		PendingRecoilYaw -= InterpYaw;
	}
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
	OffsetNum = 0;
}

void AWeaponBase::ShootTime()
{
	if (CurrentBulletNum > 0)
	{
		CurrentBulletNum--;
		// 触发UI更新
		InitBulletNumUI();
	}
	else
	{
		//弹夹空了，停止开火
		WeaponOwner->StopFire();
		return;
	}
	
	if (WeaponOwner && WeaponOwner->FireMontage)
	{
		WeaponOwner->PlayAnimMontage(WeaponOwner->FireMontage, 1.0f);
		
	}
	//播放击中特效与音效
	if (WeaponOwner && WeaponOwner->GetController())
	{
		FVector StartLocation;
		FRotator StartRotation;
		WeaponOwner->GetController()->GetPlayerViewPoint(StartLocation, StartRotation);
		FHitResult HitResult;
		bool bHit = WeaponOwner->UniDoLineTrace(StartLocation, StartRotation, 10000.0f, HitResult,true);
		
		// //添加后坐力
		// if (OffsetNum < 4)
		// {
		// 	OffsetNum++;
		// 	WeaponOwner->AddPitchOffset(-PitchOffset);
		// 	WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset, YawOffset));
		// }
		// else if(OffsetNum < WeaponDataAsset->RecoilMaxCount)
		// {
		// 	OffsetNum++;
		// 	WeaponOwner->AddPitchOffset(-PitchOffset*FMath::RandRange(2, OffsetNum));
		// 	WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
		// }
		// else
		// {
		// 	WeaponOwner->AddPitchOffset(-PitchOffset*2.0f);
		// 	WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
		// }
		
		//添加后坐力
		if (OffsetNum < 4)
		{
			OffsetNum++;
			WeaponOwner->AddPitchOffset(-PitchOffset);
			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset, YawOffset));
		}
		else if(OffsetNum < WeaponDataAsset->RecoilMaxCount)
		{
			OffsetNum++;
			WeaponOwner->AddPitchOffset(-PitchOffset*FMath::RandRange(2, OffsetNum));
			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
		}
		else
		{
			WeaponOwner->AddPitchOffset(-PitchOffset*2.0f);
			WeaponOwner->AddYawOffset(FMath::RandRange(-YawOffset*4, YawOffset*4));
		}
		
		if (bHit)
		{
			if (HitResult.PhysMaterial.Get())
			{
				switch (HitResult.PhysMaterial.Get()->SurfaceType)
				{
				case SurfaceType1:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DirtEffect, HitResult.Location, HitResult.Normal.Rotation());
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), DirtSound, HitResult.Location);
					break;
				case SurfaceType2:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GrassEffect, HitResult.Location, HitResult.Normal.Rotation());
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), GrassSound, HitResult.Location);
					break;
				case SurfaceType3:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RockEffect, HitResult.Location, HitResult.Normal.Rotation());
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), RockSound, HitResult.Location);
					break;
				case SurfaceType4:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MetalEffect, HitResult.Location, HitResult.Normal.Rotation());
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), MetalSound, HitResult.Location);
					break;
				case SurfaceType5:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WoodEffect, HitResult.Location, HitResult.Normal.Rotation());
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), WoodSound, HitResult.Location);
					break;
				default:
					break;
				}
			}
			
			// 设置较小的FadeScreenSize防止稍微离远就看不见弹孔
			FVector DecalSize(10.0f, 5.0f, 5.0f);
			if (UDecalComponent* DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, DecalSize, HitResult.Location, HitResult.Normal.Rotation(), 30.0f))
			{
				DecalComp->SetFadeScreenSize(0.005f);
			}
		}
	}
	
	//播放音效和动画
	if (WeaponDataAsset)
	{
		//播放开火动画
		if (WeaponSkeletalMesh && WeaponDataAsset->FireAnimation)
		{
			WeaponSkeletalMesh->PlayAnimation(WeaponDataAsset->FireAnimation, false);
		}
		
		if (WeaponDataAsset->FireSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), WeaponDataAsset->FireSound);
		}
	}
}

void AWeaponBase::InitBulletNumUI()
{
	GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->CurrentBulletNumChangedDelegate.Broadcast(CurrentBulletNum);
	GetGameInstance()->GetSubsystem<UDelegatesSubsystem>()->AllBulletNumChangedDelegate.Broadcast(AllBulletNum);
}
