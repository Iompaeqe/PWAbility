// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "PWSpellActor.h"
 
#include "Ability/PWSpellParams.h"
#include "Ability/PWSpellTargetProvider.h"
#include "Components/PrimitiveComponent.h"
#include "Ability/PWAffectable.h"
#include "Ability/PWEffectReceiver.h"
#include "Interface/PWProjectileInterface.h"
#include "Net/UnrealNetwork.h"

#include "TeamFunctions.h"

APWSpellActor::APWSpellActor(): SpellParams(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;
	AActor::SetReplicateMovement(true);
}

void APWSpellActor::BeginPlay()
{
	Super::BeginPlay();
	
	// AMyGameState* MyGameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	// MyGameState->OnGamePhaseChanged.AddDynamic(this, &APWSpellActor::OnGamePhaseChanged);
}

void APWSpellActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(APWSpellActor, SpellParams, COND_InitialOnly);
}

void APWSpellActor::ResetSpell()
{
}

// void APWSpellActor::OnGamePhaseChanged(EGamePhase NewPhase)
// {
// 	Destroy();
// }

void APWSpellActor::OnRep_SpellParams()
{
	bHasReplicatedData = SpellParams != nullptr;
}

void APWSpellActor::SetSpellParams_Implementation(UPWSpellParams* Params)
{
	SpellParams = Params;
	if (!SpellParams || !HasAuthority()) return;

	SpellParams->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	AddReplicatedSubObject(SpellParams);
}

bool APWSpellActor::CheckForCollisionTarget(const AActor* OtherActor) const
{
	
	return SpellParams && UTeamFunctions::MatchesMask(SpellParams->SourceAvatar, OtherActor, SpellParams->TargetCollisionMask);
}

bool APWSpellActor::CheckForEffectTarget(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::MatchesMask(SpellParams->SourceAvatar, OtherActor, SpellParams->TargetEffectMask);
}

bool APWSpellActor::IsSelf(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Self;
}

bool APWSpellActor::IsEnemy(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Enemy;
}

bool APWSpellActor::IsAlly(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Ally;
}

bool APWSpellActor::IsSameCast(const AActor* OtherActor) const
{
	const APWSpellActor* Other = Cast<APWSpellActor>(OtherActor);
	if (!Other || !SpellParams || !Other->SpellParams) return false;
	return SpellParams->CastId.IsValid() && SpellParams->CastId == Other->SpellParams->CastId;
}

bool APWSpellActor::IsActorProjectile(const AActor* OtherActor)
{
	return OtherActor->Implements<UPWProjectileInterface>();
}

bool APWSpellActor::IsNoCollision(const AActor* OtherActor)
{
	return OtherActor->Implements<UPWNoCollision>();
}

TArray<AActor*> APWSpellActor::GetImpactTargets(AActor* HitActor) const
{
	TArray<AActor*> Targets;
	if (const IPWSpellTargetProvider* Resolver = SpellParams ? Cast<IPWSpellTargetProvider>(SpellParams->TargetResolver) : nullptr)
	{
		const FHitResult Hit = HitActor ? FHitResult(HitActor, Cast<UPrimitiveComponent>(HitActor->GetRootComponent()), GetActorLocation(), -GetActorForwardVector()) : FHitResult();
		Resolver->GetImpactTargets(this, SpellParams, Hit, Targets);
		return Targets;
	}

	if (HitActor)
		Targets.Add(HitActor);
	return Targets;
}

bool APWSpellActor::IsDontTriggerCollision(const AActor* OtherActor)
{
	return OtherActor->Implements<UPWDontTriggerCollision>();
}


