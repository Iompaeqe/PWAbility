// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Components/PWProjectileMovementComponent.h"


FVector UPWProjectileMovementComponent::GetHomingTargetLocation() const
{
	return HomingTargetLocation;
}

AActor* UPWProjectileMovementComponent::GetHomingTargetActor() const
{
	return HomingTargetActor;
}

void UPWProjectileMovementComponent::SetHomingTargetLocation(const FVector& TargetLocation)
{
	USceneComponent* HomingTarget = NewObject<USceneComponent>(USceneComponent::StaticClass());
	HomingTarget->SetWorldLocation(TargetLocation);
	HomingTargetComponent = HomingTarget;
	HomingTargetLocation = TargetLocation;
}

void UPWProjectileMovementComponent::SetHomingTargetActor(AActor* TargetActor)
{
	USceneComponent* HomingTarget = nullptr;
	if (TargetActor)
		HomingTarget = TargetActor->GetRootComponent();
	HomingTargetComponent = HomingTarget;
	HomingTargetActor = TargetActor;
}

void UPWProjectileMovementComponent::AddVelocity(const FVector& DeltaForce)
{
	Velocity += DeltaForce;

	SetVelocityInLocalSpace(Velocity);
}
