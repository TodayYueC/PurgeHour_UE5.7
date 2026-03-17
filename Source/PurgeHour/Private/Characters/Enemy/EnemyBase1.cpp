// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyBase1.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Characters/Hero/Hero.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"

namespace
{
bool IsHeroTarget(const AActor* Candidate, const UWorld* World)
{
	if (!Candidate)
	{
		return false;
	}

	if (Candidate->IsA<AHero>() || Candidate->ActorHasTag(TEXT("Player")) || Candidate->ActorHasTag(TEXT("Hero")))
	{
		return true;
	}

	if (World)
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC->GetPawn() == Candidate;
		}
	}

	return false;
}
}

// Sets default values
AEnemyBase1::AEnemyBase1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RangeToChase = CreateDefaultSubobject<USphereComponent>("RangeToChase");
	RangeToChase->SetupAttachment(RootComponent);
	
	AttackCollision = CreateDefaultSubobject<UBoxComponent>("AttackCollision");
	AttackCollision->SetupAttachment(GetMesh(),FName("EnemyAttackSocket"));
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	//AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	bUseControllerRotationYaw = false;
	// 2. 开启“朝向移动方向”
	// 只有开了这个，角色才会根据 MoveTo 的路径自动转身
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 3. 设置转身速度 (RotationRate)
	// 这里的 Yaw (Z轴) 决定了转身快慢
	// 360 = 慢悠悠，720 = 敏捷，2000 = 瞬间
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true; 
}

// Called when the game starts or when spawned
void AEnemyBase1::BeginPlay()
{
	Super::BeginPlay();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AttackGA)
		{
			FGameplayAbilitySpec AttackSpec(AttackGA, 1, -1, this);
			AttackGAHandle = ASC->GiveAbility(AttackSpec);
		}
	}

	if (AttackCollision)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true; 
		AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase1::OnAttackOverlapBegin);
	}
	
}

// Called every frame
void AEnemyBase1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase1::Attack()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AttackGAHandle.IsValid())
		{
			ASC->TryActivateAbility(AttackGAHandle);
			return;
		}
	}

	// Legacy montage-based attack path is intentionally disabled after GA migration.
	// if (!MontageAttack)
	// {
	// 	return;
	// }
	//
	// UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// if (AnimInstance && !AnimInstance->Montage_IsPlaying(MontageAttack))
	// {
	// 	PlayAnimMontage(MontageAttack);
	// }
}

void AEnemyBase1::SetCollisionActive(bool bIsActive)
{
	if (AttackCollision)
	{
		if (bIsActive)
		{
			AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			HitVictims.Empty();
		}
		else
		{
			AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			HitVictims.Empty();
		}
	}
}

void AEnemyBase1::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (!IsHeroTarget(OtherActor, GetWorld()))
		{
			return;
		}

		if (!HitVictims.Contains(OtherActor))
		{
			HitVictims.Add(OtherActor);
			
			UGameplayStatics::ApplyDamage(
				OtherActor,
				20.0f,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

