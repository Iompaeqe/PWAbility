// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Library/PWSpellActorFunctions.h"

#include "NiagaraComponent.h"
#include "GameFramework/MovementComponent.h"

void UPWSpellActorFunctions::CascadedDestroy(UPrimitiveComponent* CollisionComponent, UNiagaraComponent* Effect, float Delay, bool bHideActor)
{
	AActor* Owner = CollisionComponent ? CollisionComponent->GetOwner() : Effect ? Effect->GetOwner() : nullptr;
	if (!Owner)
		return;

	if (CollisionComponent)
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Effect)
	{
		Effect->Deactivate();
		Effect->SetAutoDestroy(true);
	}
	if (UMovementComponent* Movement = Owner->FindComponentByClass<UMovementComponent>())
	{
		Movement->StopMovementImmediately();
		Movement->Deactivate();
	}
	if (bHideActor)
		Owner->SetActorHiddenInGame(true);

	if (Delay <= 0.f)
	{
		Owner->Destroy();
		return;
	}

	FTimerHandle TimerHandle;
	Owner->GetWorldTimerManager().SetTimer(TimerHandle, [WeakOwner = TWeakObjectPtr<AActor>(Owner)]()
	{
		if (WeakOwner.IsValid())
			WeakOwner->Destroy();
	}, Delay, false);
}
