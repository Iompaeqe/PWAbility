// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "WBaseSpellActor.h"
 
#include "Ability/SpellParams.h"
#include "Ability/PWSpellTargetProvider.h"
#include "Components/PrimitiveComponent.h"
#include "Ability/Affectable.h"
#include "Ability/EffectReceiver.h"
#include "Interface/PWProjectile.h"
#include "Net/UnrealNetwork.h"

#include "TeamFunctions.h"

AWBaseSpellActor::AWBaseSpellActor(): SpellParams(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;
	AActor::SetReplicateMovement(true);
}

void AWBaseSpellActor::BeginPlay()
{
	Super::BeginPlay();
	
	// AMyGameState* MyGameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	// MyGameState->OnGamePhaseChanged.AddDynamic(this, &AWBaseSpellActor::OnGamePhaseChanged);
}

void AWBaseSpellActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AWBaseSpellActor, SpellParams, COND_InitialOnly);
}

void AWBaseSpellActor::ResetSpell()
{
}

// void AWBaseSpellActor::OnGamePhaseChanged(EGamePhase NewPhase)
// {
// 	Destroy();
// }

void AWBaseSpellActor::OnRep_SpellParams()
{
	bHasReplicatedData = SpellParams != nullptr;
}

void AWBaseSpellActor::SetSpellParams_Implementation(USpellParams* Params)
{
	SpellParams = Params;
	if (!SpellParams || !HasAuthority()) return;

	SpellParams->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	AddReplicatedSubObject(SpellParams);
}

bool AWBaseSpellActor::CheckForCollisionTarget(const AActor* OtherActor) const
{
	
	return SpellParams && UTeamFunctions::MatchesMask(SpellParams->SourceAvatar, OtherActor, SpellParams->TargetCollisionMask);
}

bool AWBaseSpellActor::CheckForEffectTarget(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::MatchesMask(SpellParams->SourceAvatar, OtherActor, SpellParams->TargetEffectMask);
}

bool AWBaseSpellActor::IsSelf(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Self;
}

bool AWBaseSpellActor::IsEnemy(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Enemy;
}

bool AWBaseSpellActor::IsAlly(const AActor* OtherActor) const
{
	return SpellParams && UTeamFunctions::GetRelation(SpellParams->SourceAvatar, OtherActor) == ERelation::Ally;
}

bool AWBaseSpellActor::IsSameCast(const AActor* OtherActor) const
{
	const AWBaseSpellActor* Other = Cast<AWBaseSpellActor>(OtherActor);
	if (!Other || !SpellParams || !Other->SpellParams) return false;
	return SpellParams->CastId.IsValid() && SpellParams->CastId == Other->SpellParams->CastId;
}

bool AWBaseSpellActor::IsActorProjectile(const AActor* OtherActor)
{
	return OtherActor->Implements<UPWProjectile>();
}

bool AWBaseSpellActor::IsNoCollision(const AActor* OtherActor)
{
	return OtherActor->Implements<UNoCollision>();
}

TArray<AActor*> AWBaseSpellActor::GetImpactTargets(AActor* HitActor) const
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

bool AWBaseSpellActor::IsDontTriggerCollision(const AActor* OtherActor)
{
	return OtherActor->Implements<UDontTriggerCollision>();
}


