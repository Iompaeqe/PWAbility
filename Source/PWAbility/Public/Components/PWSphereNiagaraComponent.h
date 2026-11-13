// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PWSphereNiagaraComponent.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class PWABILITY_API UPWSphereNiagaraComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPWSphereNiagaraComponent();

protected:

	// Effect shown for the sphere.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	// Sphere that blocks movement.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> BlockComponent;
	// Sphere that reports overlaps.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> OverlapComponent;

public:
};
