// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PWSpellActorFunctions.generated.h"

class UNiagaraComponent;
UCLASS()
class PWABILITY_API UPWSpellActorFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Retires a spent spell actor: turns off collision, stops its movement and effect, hides it and destroys it after the delay. With Hide Actor off it stays in view while its effect fades.
	UFUNCTION(BlueprintCallable, Category = "AbilityActor")
	static void CascadedDestroy(UPrimitiveComponent* CollisionComponent, UNiagaraComponent* Effect, float Delay = 0, bool bHideActor = true);
};
