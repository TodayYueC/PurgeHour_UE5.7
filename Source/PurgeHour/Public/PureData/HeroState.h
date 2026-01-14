#pragma once

#include "CoreMinimal.h"
#include "HeroState.generated.h"

UENUM(BlueprintType)
enum class EHeroState : uint8
{
	EmptyHanded UMETA(DisplayName = "EmptyHanded"),
	HoldingWeapon UMETA(DisplayName = "HoldingWeapon"),
};