// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "PWEffectActor.h"

#include "PWAbilityEnums.h"

APWEffectActor::APWEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneComponent"));
}

void APWEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void APWEffectActor::ApplyEffectToTarget(AActor* TargetActor, FPWEffectData EffectData)
{
	// if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies) return;
	//
	// UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// if (!TargetASC) return;
	// check(EffectData.GameplayEffectClass)
	//
	// FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	// EffectContextHandle.AddSourceObject(this);
	// const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
	// 	EffectData.GameplayEffectClass, ActorLevel, EffectContextHandle);
	// const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(
	// 	*EffectSpecHandle.Data.Get());
	//
	// const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	// if (bIsInfinite && EffectData.EffectRemovalPolicy != EPWEffectRemovalPolicy::DoNotRemove)
	// {
	// 	ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	// }
	//
	// if (!bIsInfinite && bDestroyOnEffectApplication)
	// {
	// 	Destroy();
	// }
}

void APWEffectActor::RemoveEffectFromTarget(AActor* TargetActor)
{
	// UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// if (!IsValid(TargetASC)) return;
	//
	// TArray<FActiveGameplayEffectHandle> HandlesToRemove;
	// for (auto HandlePair : ActiveEffectHandles)
	// {
	// 	if (TargetASC == HandlePair.Value)
	// 	{
	// 		TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
	// 		HandlesToRemove.Add(HandlePair.Key);
	// 	}
	// }
	// 	
	// for (auto& Handle : HandlesToRemove)
	// {
	// 	ActiveEffectHandles.FindAndRemoveChecked(Handle);
	// }
}

void APWEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies) return;
	
	for (FPWEffectData Effect : Effects)
	{
		if (Effect.EffectApplicationPolicy == EPWEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor,Effect);
		}
	}
}

void APWEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies) return;
	
	for (FPWEffectData Effect : Effects)
	{
		if (Effect.EffectApplicationPolicy == EPWEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor,Effect);
		}
	}

	for (FPWEffectData Effect : Effects)
	{
		if (Effect.EffectRemovalPolicy == EPWEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			RemoveEffectFromTarget(TargetActor);
		}
	}
}
