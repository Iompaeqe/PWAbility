// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PWAbilityEnums.generated.h"

UENUM(BlueprintType)
enum class EPWProjectileVsProjectilePolicy : uint8
{
	Ignore,
	DestroySelf,
	BounceOff
};

UENUM(BlueprintType)
enum class EPWEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EPWEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};


UCLASS()
class PWABILITY_API UPWAbilityEnums : public UObject
{
	GENERATED_BODY()
};
