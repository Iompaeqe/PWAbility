// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "BaseProjectile.h"

#include "AbilityEnums.h"
#include "Ability/SpellParams.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PWAbility.h"
#include "Ability/Affectable.h"
#include "Ability/EffectReceiver.h"

ABaseProjectile::ABaseProjectile()
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

	ProjectileMovement = CreateDefaultSubobject<UWProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnBlockingHit);
	LoopingSound = UGameplayStatics::SpawnSoundAttached(ActiveSoundLoop, GetRootComponent());
}

void ABaseProjectile::Destroyed()
{
	Super::Destroyed();

	Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &ABaseProjectile::OnOverlap);
	Sphere->OnComponentHit.RemoveDynamic(this, &ABaseProjectile::OnBlockingHit);

	if (LoopingSound)
	{
		LoopingSound->Stop();
		LoopingSound->DestroyComponent();
	}
}

bool ABaseProjectile::ShouldIgnoreOverlap(const AActor* OtherActor) const
{
	if (IsNoCollision(this))
		return true;

	if (IsDontTriggerCollision(OtherActor))
		return true;
	if (IsSameCast(OtherActor))
		return true;

	return false;
}

bool ABaseProjectile::CanHitTarget(const AActor* OtherActor)
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

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (ShouldIgnoreOverlap(OtherActor)) return;
	if (IsActorProjectile(OtherActor))
	{
		const AWBaseSpellActor* OtherSpell = Cast<AWBaseSpellActor>(OtherActor);
		if (HasReplicatedData() && (!OtherSpell || OtherSpell->HasReplicatedData()))
			HandleProjectileCollision(OtherActor);
		return;
	}
	if (OtherComp && OtherComp->Implements<UDontTriggerCollision>()) return;
	// A volume that blocks neither characters nor projectiles, such as a trigger, is not something to hit.
	if (OtherComp && OtherComp->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Block && OtherComp->GetCollisionResponseToChannel(Sphere->GetCollisionObjectType()) != ECR_Block) return;
	if (!CanHitTarget(OtherActor)) return;
	if (!SpellParams || !HasReplicatedData()) return;

	// The mask chooses between targets. Anything else is scenery, such as a wall, and always triggers the impact.
	if (FindAffectable(OtherActor) && !CheckForCollisionTarget(OtherActor)) return;

	LastHitActor = OtherActor;
	LastHitTimes.FindOrAdd(OtherActor) = GetWorld()->GetTimeSeconds();
	HitCount++;

	if (HitCount == DestroyOnHit)
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnProjectileOverlap(OtherActor);
}

void ABaseProjectile::OnBlockingHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// A projectile set to block something impacts on it the same way as on an overlap.
	OnOverlap(HitComponent, OtherActor, OtherComp, 0, true, Hit);
}

void ABaseProjectile::OnRep_SpellParams()
{
	Super::OnRep_SpellParams();

	TArray<AActor*> OverlappingActors;
	Sphere->GetOverlappingActors(OverlappingActors, ABaseProjectile::StaticClass());
	for (AActor* OtherActor : OverlappingActors)
	{
		ABaseProjectile* Other = Cast<ABaseProjectile>(OtherActor);
		OnOverlap(Sphere, Other, Other->Sphere, 0, false, FHitResult());
		Other->OnOverlap(Other->Sphere, this, Sphere, 0, false, FHitResult());
	}
}

void ABaseProjectile::HandleProjectileCollision(const AActor* OtherActor)
{
	switch (ProjectileVsProjectilePolicy)
	 {
	 case EProjectileVsProjectilePolicy::Ignore:
	 	return;
	
	 case EProjectileVsProjectilePolicy::DestroySelf:
	 	HandleImpactEffects();
	 	if (HasAuthority()) Destroy();
	 	return;
	
	 case EProjectileVsProjectilePolicy::BounceOff:
	 	Bounce(OtherActor->GetActorLocation());
	 	return;
	 }
}

void ABaseProjectile::SimpleApplyEffectsAndDestroy(AActor* OtherActor)
{
	SimpleApplyEffects(OtherActor);

	if (HasAuthority() && ShouldDestroy())
	{
		Destroy();
	}
}

void ABaseProjectile::SimpleApplyEffects(AActor* OtherActor)
{
	HandleImpactEffects();

	if (HasAuthority())
	{
		if (CheckForEffectTarget(OtherActor))
			ApplyEffects(OtherActor);
	}
}

void ABaseProjectile::ApplyEffects(AActor* OtherActor) const
{
	if (!HasAuthority()) return;

	SpellParams->ActorHit = OtherActor;
	
	if (UObject* Receiver = FindAffectable(OtherActor))
		IAffectable::Execute_ApplyEffects(Receiver, SpellParams);
}

void ABaseProjectile::HandleImpactEffects()
{
	if (HasAuthority())
		MulticastPlayImpactEffects();
	else
		PlayImpactEffects();
}

void ABaseProjectile::PlayImpactEffects()
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

void ABaseProjectile::MulticastPlayImpactEffects_Implementation()
{
	PlayImpactEffects();
}

void ABaseProjectile::Bounce(const FVector PointOfOrigin) const
{
	ProjectileMovement->Velocity = CalculateBounceVelocity(PointOfOrigin, ProjectileMovement->Velocity);
	ProjectileMovement->UpdateComponentVelocity();
}

void ABaseProjectile::ResetSpell()
{
	Super::ResetSpell();

	bHitEffectsPlayed = false;
	LastHitActor = nullptr;
	LastHitTimes.Empty();
	HitCount = 0;
}

FVector ABaseProjectile::CalculateBounceVelocity(const FVector PointOfOrigin, const FVector Velocity) const
{
	FVector SphereOrigin = PointOfOrigin;
	SphereOrigin.Z = GetActorLocation().Z;
	const FVector Normal = (GetActorLocation() - SphereOrigin).GetSafeNormal();
	const FVector ReflectedVelocity = FMath::GetReflectionVector(Velocity, Normal);
	return ReflectedVelocity;
}
