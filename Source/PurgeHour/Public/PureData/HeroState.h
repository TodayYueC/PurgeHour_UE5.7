#pragma once

#include "CoreMinimal.h"
#include "HeroState.generated.h"

UENUM(BlueprintType)
enum class EHeroState : uint8
{
	EmptyHanded UMETA(DisplayName = "EmptyHanded"),
	HoldingWeapon UMETA(DisplayName = "HoldingWeapon"),
	HoldingSword UMETA(DisplayName = "HoldingSword"),
};

UENUM(BlueprintType)
enum class EHeroMoveDirection : uint8
{
	None UMETA(DisplayName = "None"),
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class EHeroFacingMode : uint8
{
	OrientToMovement UMETA(DisplayName = "OrientToMovement"),
	FaceController UMETA(DisplayName = "FaceController"),
};

