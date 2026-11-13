// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "PWProjectile.h"

#include "PWAbilityEnums.h"
#include "Ability/PWSpellParams.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PWProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PWAbility.h"
#include "Ability/PWAffectable.h"
#include "Ability/PWEffectReceiver.h"

APWProjectile::APWProjectile()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	//Sphere->SetCollisionResponseToChannel(ECC_PawnOverlap, ECR_Overlap);
	SetRootComponent(Sphere);

	ProjectileMovement = CreateDefaultSubobject<UPWProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0;
}

void APWProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APWProjectile::OnOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &APWProjectile::OnBlockingHit);
	LoopingSound = UGameplayStatics::SpawnSoundAttached(ActiveSoundLoop, GetRootComponent());
}

void APWProjectile::Destroyed()
{
	Super::Destroyed();

	Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &APWProjectile::OnOverlap);
	Sphere->OnComponentHit.RemoveDynamic(this, &APWProjectile::OnBlockingHit);

	if (LoopingSound)
	{
		LoopingSound->Stop();
		LoopingSound->DestroyComponent();
	}
}

bool APWProjectile::ShouldIgnoreOverlap(const AActor* OtherActor) const
{
	if (IsNoCollision(this))
		return true;

	if (IsDontTriggerCollision(OtherActor))
		return true;
	if (IsSameCast(OtherActor))
		return true;

	return false;
}

bool APWProjectile::CanHitTarget(const AActor* OtherActor)
{
	if (!bCanHitTheSameTarget)
	{
		if (LastHitActor == OtherActor)
			return false;
	}

	const float* LastTime = LastHitTimes.Find(OtherActor);
	float Now = GetWorld()->GetTimeSeconds();
	return (!LastTime || Now - *LastTime >= TargetHitCooldown);
}

void APWProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (ShouldIgnoreOverlap(OtherActor)) return;
	if (IsActorProjectile(OtherActor))
	{
		const APWSpellActor* OtherSpell = Cast<APWSpellActor>(OtherActor);
		if (HasReplicatedData() && (!OtherSpell || OtherSpell->HasReplicatedData()))
			HandleProjectileCollision(OtherActor);
		return;
	}
	if (OtherComp && OtherComp->Implements<UPWDontTriggerCollision>()) return;
	if (!CanHitTarget(OtherActor)) return;
	if (!SpellParams || !HasReplicatedData()) return;

	// The mask chooses between targets. Anything else is scenery, such as a wall, and always triggers the impact.
	if (IPWAffectable::Find(OtherActor) && !CheckForCollisionTarget(OtherActor)) return;

	LastHitActor = OtherActor;
	LastHitTimes.FindOrAdd(OtherActor) = GetWorld()->GetTimeSeconds();
	HitCount++;

	if (HitCount == DestroyOnHit)
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnProjectileOverlap(OtherActor);
}

void APWProjectile::OnBlockingHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// A projectile set to block something impacts on it the same way as on an overlap.
	OnOverlap(HitComponent, OtherActor, OtherComp, 0, true, Hit);
}

void APWProjectile::OnRep_SpellParams()
{
	Super::OnRep_SpellParams();

	TArray<AActor*> OverlappingActors;
	Sphere->GetOverlappingActors(OverlappingActors, APWProjectile::StaticClass());
	for (AActor* OtherActor : OverlappingActors)
	{
		APWProjectile* Other = Cast<APWProjectile>(OtherActor);
		OnOverlap(Sphere, Other, Other->Sphere, 0, false, FHitResult());
		Other->OnOverlap(Other->Sphere, this, Sphere, 0, false, FHitResult());
	}
}

void APWProjectile::HandleProjectileCollision(const AActor* OtherActor)
{
	switch (ProjectileVsProjectilePolicy)
	 {
	 case EPWProjectileVsProjectilePolicy::Ignore:
	 	return;
	
	 case EPWProjectileVsProjectilePolicy::DestroySelf:
	 	HandleImpactEffects();
	 	if (HasAuthority()) Destroy();
	 	return;
	
	 case EPWProjectileVsProjectilePolicy::BounceOff:
	 	Bounce(OtherActor->GetActorLocation());
	 	return;
	 }
}

void APWProjectile::SimpleApplyEffectsAndDestroy(AActor* OtherActor)
{
	SimpleApplyEffects(OtherActor);

	if (HasAuthority() && ShouldDestroy())
	{
		Destroy();
	}
}

void APWProjectile::SimpleApplyEffects(AActor* OtherActor)
{
	HandleImpactEffects();

	if (HasAuthority())
	{
		for (AActor* Target : GetImpactTargets(OtherActor))
			if (CheckForEffectTarget(Target))
				ApplyEffects(Target);
	}
}

void APWProjectile::ApplyEffects(AActor* OtherActor) const
{
	if (!HasAuthority()) return;

	SpellParams->ActorHit = OtherActor;
	
	if (UObject* Receiver = IPWAffectable::Find(OtherActor))
		IPWAffectable::Execute_ApplyEffects(Receiver, SpellParams);
}

void APWProjectile::HandleImpactEffects()
{
	if (HasAuthority())
		MulticastPlayImpactEffects();
	else
		PlayImpactEffects();
}

void APWProjectile::PlayImpactEffects()
{
	if (bHitEffectsPlayed) return;
	bHitEffectsPlayed = true;

	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	if (ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());

	if (ShouldDestroy() && LoopingSound)
	{
		LoopingSound->Stop();
		LoopingSound->DestroyComponent();
	}
}

void APWProjectile::MulticastPlayImpactEffects_Implementation()
{
	PlayImpactEffects();
}

void APWProjectile::Bounce(const FVector PointOfOrigin) const
{
	ProjectileMovement->Velocity = CalculateBounceVelocity(PointOfOrigin, ProjectileMovement->Velocity);
	ProjectileMovement->UpdateComponentVelocity();
}

void APWProjectile::ResetSpell()
{
	Super::ResetSpell();

	bHitEffectsPlayed = false;
	LastHitActor = nullptr;
	LastHitTimes.Empty();
	HitCount = 0;
}

FVector APWProjectile::CalculateBounceVelocity(const FVector PointOfOrigin, const FVector Velocity) const
{
	FVector SphereOrigin = PointOfOrigin;
	SphereOrigin.Z = GetActorLocation().Z;
	const FVector Normal = (GetActorLocation() - SphereOrigin).GetSafeNormal();
	const FVector ReflectedVelocity = FMath::GetReflectionVector(Velocity, Normal);
	return ReflectedVelocity;
}
