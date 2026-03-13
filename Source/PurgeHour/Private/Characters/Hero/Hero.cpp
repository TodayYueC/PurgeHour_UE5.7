// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/Hero.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/SwordBase.h"
#include "AbilitySystem/HeroYueASC.h"
#include "System/HeroPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/GA/BaseWeaponGA.h"
#include "AbilitySystem/GA/BaseSwordGA.h"
#include "MotionWarpingComponent.h"
#include "Engine/Engine.h"
#include "Characters/EnemyBase.h"
#include "Math/RotationMatrix.h"

AHero::AHero()
{
	// Hero 需要 Tick（CharacterBase 已关闭，此处单独开启）
	PrimaryActorTick.bCanEverTick = true;
	
	//////////////////////////////摄像机与弹簧臂组件//////////////////////////////////
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 250.0f; // 弹簧臂长度
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComponent->SocketOffset = FVector(0.0f, 65.0f, 40.0f); // 偏移位置
	SpringArmComponent->bUsePawnControlRotation = true; // 随控制器旋转，处理视角上下移动
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // 摄像机不随控制器旋转
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//////////////////////////////操作手感优化//////////////////////////////////
	JumpMaxHoldTime = 0.2f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void AHero::BeginPlay()
{
	Super::BeginPlay();
	CurrentHeroState = EHeroState::EmptyHanded;
	ApplyFacingModeByState();
	Init();
}

void AHero::Init()
{
	InitGEToSelf();
}

void AHero::InitGEToSelf()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent || !InitGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System Component Or InitGE is NULL"));
		return;
	}
	
	FGameplayEffectContextHandle InitContextHandle = AbilitySystemComponent->MakeEffectContext();
	InitContextHandle.AddInstigator(this,this);
	
	FGameplayEffectSpecHandle InitSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitGE, 1, InitContextHandle);
	if (InitSpecHandle.IsValid())
	{		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitSpecHandle.Data.Get());
	}
}

void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ---- 后坐力施加：在 RecoilApplyTime 秒内线性消耗完 Pending ----
	if (!FMath::IsNearlyZero(PendingRecoilPitch, 0.001f) && RecoilPitchTimeRemaining > 0.f)
	{
		const float ApplyPitch = PendingRecoilPitch * FMath::Min(DeltaTime / RecoilPitchTimeRemaining, 1.0f);
		AddControllerPitchInput(-ApplyPitch);
		PendingRecoilPitch -= ApplyPitch;
		RecoilPitchTimeRemaining -= DeltaTime;
		if (RecoilPitchTimeRemaining <= 0.f || FMath::Abs(PendingRecoilPitch) < 0.001f)
		{
			PendingRecoilPitch = 0.f;
			RecoilPitchTimeRemaining = 0.f;
		}
	}

	if (!FMath::IsNearlyZero(PendingRecoilYaw, 0.001f) && RecoilYawTimeRemaining > 0.f)
	{
		const float ApplyYaw = PendingRecoilYaw * FMath::Min(DeltaTime / RecoilYawTimeRemaining, 1.0f);
		AddControllerYawInput(ApplyYaw);
		PendingRecoilYaw -= ApplyYaw;
		RecoilYawTimeRemaining -= DeltaTime;
		if (RecoilYawTimeRemaining <= 0.f || FMath::Abs(PendingRecoilYaw) < 0.001f)
		{
			PendingRecoilYaw = 0.f;
			RecoilYawTimeRemaining = 0.f;
		}
	}

	// ---- 后坐力计数自动恢复：停止射击超过 Delay 后，计数按 Speed 逐帧递减 ----
	if (RecoilShotCount > 0.f)
	{
		TimeSinceLastShot += DeltaTime;
		if (TimeSinceLastShot >= RecoilRecoveryDelay)
		{
			RecoilShotCount = FMath::Max(0.f, RecoilShotCount - RecoilRecoverySpeed * DeltaTime);
		}
	}
}

void AHero::PickUpWeapon(AWeaponBase* NewWeapon)
{
	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;
		SetCurrentHeroState(EHeroState::HoldingWeapon);
		NewWeapon->SetActorEnableCollision(false);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (ASC && CurrentWeapon->GetGAFireClass())
		{
			FGameplayAbilitySpec FireSpec(CurrentWeapon->GetGAFireClass(), 1, -1, CurrentWeapon);
			FireSpec.GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Fire")));
			GAFireHandle = ASC->GiveAbility(FireSpec);
		}
		if (ASC && CurrentWeapon->GetGAReloadClass())
		{
			FGameplayAbilitySpec ReloadSpec(CurrentWeapon->GetGAReloadClass(), 1, -1, CurrentWeapon);
			GAReloadHandle = ASC->GiveAbility(ReloadSpec);
		}
	}
}

void AHero::PickUpSword(ASwordBase* NewSword)
{
	if (!NewSword) return;

	CurrentSword = NewSword;
	SetCurrentHeroState(EHeroState::HoldingSword);

	// 禁用碰撞，防止再次触发重叠
	NewSword->SetActorEnableCollision(false);

	// 吸附到 Sword 插槽
	CurrentSword->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("SwordSocket"));
	
	//GAS
	FGameplayAbilitySpec AttackSpec(CurrentSword->GetGAAttackClass(), 1, -1, CurrentSword);
	GAAttackHandle = GetAbilitySystemComponent()->GiveAbility(AttackSpec);
	
	FGameplayAbilitySpec ComboSpec(CurrentSword->GetGAComboClass(), 1, -1, CurrentSword);
	GAComboHandle = GetAbilitySystemComponent()->GiveAbility(ComboSpec);
}

void AHero::SetCurrentHeroState(EHeroState NewState)
{
	CurrentHeroState = NewState;
	ApplyFacingModeByState();
}

void AHero::ToggleFacingMode()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon)
	{
		return;
	}

	bControllerFacingModeForMelee = !bControllerFacingModeForMelee;
	ApplyFacingModeByState();
}

void AHero::ApplyFacingModeByState()
{
	if (CurrentHeroState == EHeroState::HoldingWeapon)
	{
		// 持枪时始终朝控制器方向
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		return;
	}

	// 空手/持剑时根据切换模式决定
	bUseControllerRotationYaw = bControllerFacingModeForMelee;
	GetCharacterMovement()->bOrientRotationToMovement = !bControllerFacingModeForMelee;
}

EHeroFacingMode AHero::GetCurrentFacingMode() const
{
	// 持枪时强制朝控制器，忽略切换开关
	if (CurrentHeroState == EHeroState::HoldingWeapon)
	{
		return EHeroFacingMode::FaceController;
	}

	return bControllerFacingModeForMelee
		? EHeroFacingMode::FaceController
		: EHeroFacingMode::OrientToMovement;
}

void AHero::Fire()
{
	if (CurrentHeroState != EHeroState::HoldingWeapon || !CurrentWeapon) return;
	
		// 开火已整合到GA，仅设置状态标记
		bIsFiring = true;
		
		// 以下调用武器开火的逻辑已整合到GA，注释保留
		// if (CurrentWeapon->GetCurrentBulletNum()<=0)
		// {
		// 	UGameplayStatics::PlaySound2D(GetWorld(), EmptyMagazineSound);
		// }
		// CurrentWeapon->Fire();
		if (GetAbilitySystemComponent() && GAFireHandle.IsValid())
		{
			FGameplayAbilitySpec* Spec = GetAbilitySystemComponent()->FindAbilitySpecFromHandle(GAFireHandle);
			if (Spec)
			{
				Spec->InputPressed = true;
			}
			GetAbilitySystemComponent()->TryActivateAbility(GAFireHandle);
		}
	
}

void AHero::Reload()
{
	if (CurrentHeroState != EHeroState::HoldingWeapon || !CurrentWeapon) return;
	
	if (GetAbilitySystemComponent() && GAReloadHandle.IsValid())
	{
		GetAbilitySystemComponent()->TryActivateAbility(GAReloadHandle);
	}
}

void AHero::StopFire()
{
	if (CurrentHeroState != EHeroState::HoldingWeapon || !CurrentWeapon) return;
	
		// 开火已整合到GA，仅清除状态标记
		bIsFiring = false;
		if (GetAbilitySystemComponent() && GAFireHandle.IsValid())
		{
			FGameplayAbilitySpec* FireSpec = GetAbilitySystemComponent()->FindAbilitySpecFromHandle(GAFireHandle);
			if (FireSpec)
			{
				FireSpec->InputPressed = false; // 取消输入标记，触发GA中停止开火的逻辑
				
				GetAbilitySystemComponent()->CancelAbilityHandle(GAFireHandle); // 直接取消技能，确保开火停止
			}
		}
		
		
		// 以下调用武器停火的逻辑已整合到GA，注释保留
		// CurrentWeapon->StopFire();
	
}

void AHero::ApplyRecoil(float HorizontalRecoil, float VerticalRecoil, int32 MaxRecoilCount)
{
	// 递增连发计数（float，方便Tick里平滑恢复）
	RecoilShotCount += 1.f;

	const float MaxCount = static_cast<float>(MaxRecoilCount);

	// ---- 1. 斜坡系数：前几发后坐力非常小，第4发才到达满值 ----
	// ShotCount=1 → ~0.08，ShotCount=2 → ~0.25，ShotCount=3 → ~0.55，ShotCount=4+ → 1.0
	// 使用 1 - exp(-k*(n-1)) 曲线，k=0.7 时收敛较快
	const float RampAlpha = 1.f - FMath::Exp(-0.7f * (RecoilShotCount - 1));

	// ---- 2. 超过 MaxRecoilCount 后垂直后坐力大幅衰减（枪在抖但不再明显上移） ----
	float VerticalScale = 1.f;
	if (RecoilShotCount > MaxCount)
	{
		VerticalScale = 0.15f;
	}

	// ---- 3. 水平随机：超上限后随机漂移增大，模拟枪械失控 ----
	const float HorizontalRandomSign = (FMath::RandBool() ? 1.f : -1.f);
	float HorizontalScale = 1.f;
	if (RecoilShotCount > MaxCount)
	{
		HorizontalScale = 2.5f;
	}

	// ---- 4. 累加到 Pending，同时重置时间窗口（每发子弹都在 RecoilApplyTime 内施加完） ----
	PendingRecoilPitch += VerticalRecoil * RampAlpha * VerticalScale;
	PendingRecoilYaw   += HorizontalRecoil * RampAlpha * HorizontalScale * HorizontalRandomSign;
	RecoilPitchTimeRemaining = RecoilApplyTime;
	RecoilYawTimeRemaining   = RecoilApplyTime;
	// 每次开枪重置计时，阻止恢复
	TimeSinceLastShot = 0.f;
}

void AHero::ResetRecoil()
{
	// 只清空本次射击的 Pending 后坐力，不重置计数
	// RecoilShotCount 由 Tick 根据 RecoilRecoveryDelay/Speed 自动恢复
	// 这样停止开枪再开枪，后坐力依然接着上次的计数继续
	PendingRecoilPitch       = 0.f;
	PendingRecoilYaw         = 0.f;
	RecoilPitchTimeRemaining = 0.f;
	RecoilYawTimeRemaining   = 0.f;
}

void AHero::UpdateWarpTarget()
{
		
	float TraceDistance = 350.f;
	
	FVector MyLocation = GetActorLocation();
	FVector MyDirection = GetActorForwardVector();
	
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), MyLocation, TraceDistance, ObjectTypes, nullptr, IgnoreActors, OutActors);
	
	AActor* ClosestActor = nullptr;
	float ClosestDistance = TraceDistance;
	
	for (AActor* Actor : OutActors)
	{
		// 跳过已死亡的怪物
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(Actor))
		{
			if (Enemy->IsDead()) continue;
		}

		FVector DirectionToActor = (Actor->GetActorLocation() - MyLocation).GetSafeNormal();
		
		float DotProduct = FVector::DotProduct(MyDirection, DirectionToActor);
		if (DotProduct > 0.5f) // 只考虑前方约120度范围内的目标
		{
			float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	
	
	
	if (ClosestActor && MotionWarpingComponent)
	{
		FVector TargetLocation = ClosestActor->GetActorLocation();
		FVector DirectionToMe = (MyLocation - TargetLocation).GetSafeNormal();
		FVector WarpLocation = TargetLocation + DirectionToMe * 80.f; // 在目标前方100单位处设置 Warp 位置
		
		FRotator WarpRotater = (TargetLocation - MyLocation).Rotation();
		WarpRotater.Pitch = 0.f;
		
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("CombatTarget"),WarpLocation, WarpRotater);
	
	}
	else
	{
		MotionWarpingComponent->RemoveWarpTarget(FName("CombatTarget"));
	}
}

void AHero::AddPitchOffset(float PitchOffsetAmount)
{
	AddControllerPitchInput(PitchOffsetAmount);
}

void AHero::AddYawOffset(float YawOffsetAmount)
{
	AddControllerYawInput(YawOffsetAmount);
}

UAbilitySystemComponent* AHero::GetAbilitySystemComponent() const
{
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		return HeroPlayerState->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AHero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		HeroPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(HeroPlayerState, this);
		GrantDodgeAbility();
	}
}

void AHero::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AHeroPlayerState* HeroPlayerState = GetPlayerState<AHeroPlayerState>();
	if (HeroPlayerState)
	{
		HeroPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(HeroPlayerState, this);
		GrantDodgeAbility();
	}
}

void AHero::Melee()
{
	if (CurrentHeroState != EHeroState::HoldingSword || !CurrentSword) return;
	
	if (GetAbilitySystemComponent() && GAAttackHandle.IsValid())
	{
		
			GetAbilitySystemComponent()->TryActivateAbility(GAAttackHandle);
		
	}
}

void AHero::Combo()
{
	if (CurrentHeroState != EHeroState::HoldingSword || !CurrentSword) return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && GAComboHandle.IsValid())
	{
		FGameplayAbilitySpec* Spec = ASC -> FindAbilitySpecFromHandle(GAComboHandle);
	
		if (Spec && Spec->IsActive())
		{
			FGameplayEventData GameplayEventData;
			ASC->HandleGameplayEvent(FGameplayTag ::RequestGameplayTag(FName("Input.Combo")), &GameplayEventData);
		}
		else
		{
			GetAbilitySystemComponent()->TryActivateAbility(GAComboHandle);
		}
	}
}

void AHero::Dodge()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	if (!GADodgeHandle.IsValid())
	{
		GrantDodgeAbility();
	}

	if (GADodgeHandle.IsValid())
	{
		ASC->TryActivateAbility(GADodgeHandle);
	}
}

void AHero::GrantDodgeAbility()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !GADodgeClass)
	{
		return;
	}

	if (GADodgeHandle.IsValid() && ASC->FindAbilitySpecFromHandle(GADodgeHandle))
	{
		return;
	}

	FGameplayAbilitySpec DodgeSpec(GADodgeClass, 1, -1, this);
	GADodgeHandle = ASC->GiveAbility(DodgeSpec);
}

EHeroMoveDirection AHero::GetMoveDirectionByView() const
{
	FVector HorizontalVelocity = GetVelocity();
	HorizontalVelocity.Z = 0.f;

	if (HorizontalVelocity.SizeSquared() < FMath::Square(MoveDirectionMinSpeed))
	{
		return EHeroMoveDirection::None;
	}

	const FVector MoveDir = HorizontalVelocity.GetSafeNormal();

	FRotator ViewRotation = GetActorRotation();
	if (Controller)
	{
		ViewRotation = Controller->GetControlRotation();
	}
	ViewRotation.Pitch = 0.f;
	ViewRotation.Roll = 0.f;

	const FVector ViewForward = FRotationMatrix(ViewRotation).GetUnitAxis(EAxis::X);
	const FVector ViewRight = FRotationMatrix(ViewRotation).GetUnitAxis(EAxis::Y);
	const float ForwardDot = FVector::DotProduct(MoveDir, ViewForward);
	const float RightDot = FVector::DotProduct(MoveDir, ViewRight);


	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? EHeroMoveDirection::Forward : EHeroMoveDirection::Backward;
	}
                                     
	return RightDot >= 0.f ? EHeroMoveDirection::Right : EHeroMoveDirection::Left;
}

