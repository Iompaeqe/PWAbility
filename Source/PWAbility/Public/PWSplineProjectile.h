// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "PWProjectile.h"
#include "PWSplineProjectile.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSplinePathCompleteDelegate);

UCLASS(BlueprintType, Blueprintable)
class PWABILITY_API APWSplineProjectile : public APWProjectile
{
	GENERATED_BODY()

public:
	APWSplineProjectile();

	UPROPERTY(BlueprintAssignable)
	FSplinePathCompleteDelegate OnSplinePathComplete;

	UFUNCTION(BlueprintCallable)
	void EndSplineMovement(bool ContinueMovement);
	UFUNCTION(BlueprintImplementableEvent)
	void SplineMovementEnded();
	UFUNCTION(BlueprintCallable)
	void EndSplineMovementWithBounce(const FVector PointOfOrigin);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Movement")
	USplineComponent* SplineComponent;

	// Destroys the projectile when it reaches the end of its spline.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool DestroyOnPathComplete = true;

	// Speed multiplier along the path; the curve's time runs from 0 at the start of the spline to 1 at its end.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
	FRuntimeFloatCurve CustomProjectileSpeedMultiplier;
	// Shortest path the projectile will fly, as a fraction of its range.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
	float MinimumRangeMultiplier = .5;
	// Turns the projectile to face along the spline as it moves.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Movement")
	bool bUpdateRotation = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Movement")
	float DistanceTravelled = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Movement")
	float SpeedScaleFactor = 0;

	bool bIsPathComplete = false;
	bool bIsSplineStopped = false;

	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Movement")
	bool bIsReflected = false;

private:

public:
};
