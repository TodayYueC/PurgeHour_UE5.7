// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/SwordBase.h"

#include "Components/SphereComponent.h"
#include "Data/SwordData.h"
#include "Characters/Hero/Hero.h"
#include "System/HeroPlayerState.h"

// Sets default values
ASwordBase::ASwordBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// SceneComponent 作为根组件
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(Root);

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(Root);
	PickupCollision->SetSphereRadius(100.f);
	PickupCollision->SetCollisionProfileName("OverlapAllDynamic") ;
	// PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// PickupCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// PickupCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	PickupCollision->SetGenerateOverlapEvents(true);
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ASwordBase::OnPickupBeginOverlap);
}

// Called when the game starts or when spawned
void ASwordBase::BeginPlay()
{
	Super::BeginPlay();

	// 强制在运行时设置碰撞（防止蓝图子类缓存旧值覆盖 C++ 默认值）
	// if (PickupCollision)
	// {
	// 	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 	PickupCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// 	PickupCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	// 	PickupCollision->SetGenerateOverlapEvents(true);
	// }
}

void ASwordBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 编辑器中切换 DataAsset 时自动更新网格体预览
	if (SwordDataAsset)
	{
		SwordMesh->SetStaticMesh(SwordDataAsset->StaticMesh);
	}
}

void ASwordBase::OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	SwordOwner = Cast<AHero>(OtherActor);
	if (SwordOwner)
	{
		SwordOwner->PickUpSword(this);

		// 广播剑名到 UI（与 WeaponBase 保持一致）
		if (SwordDataAsset)
		{
			if (AHeroPlayerState* PS = SwordOwner->GetPlayerState<AHeroPlayerState>())
			{
				PS->BroadcastWeaponName(SwordDataAsset->SwordDisplayName);
			}
		}

		PickupCollision->DestroyComponent();
	}
}

