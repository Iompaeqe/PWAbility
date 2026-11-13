// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "PWAbilityEnums.h"
#include "GameFramework/Actor.h"
#include "PWEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FPWEffectData
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSubclassOf<UGameplayEffect> GameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPWEffectApplicationPolicy EffectApplicationPolicy = EPWEffectApplicationPolicy::DoNotApply;
	// When an effect this actor applied is removed again.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPWEffectRemovalPolicy EffectRemovalPolicy = EPWEffectRemovalPolicy::DoNotRemove;
};

UCLASS()
class PWABILITY_API APWEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APWEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, FPWEffectData GameplayEffectClass);
	void RemoveEffectFromTarget(AActor* TargetActor);
	
	// Destroys the actor after it has applied its effects once.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects")
	bool bDestroyOnEffectApplication = false;
	
	// Lets the effects apply to actors tagged as enemies.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects")
	bool bApplyEffectToEnemies = false;

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	// Effects the actor applies, each with its own apply and remove timing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects")
	TArray<FPWEffectData> Effects;
	
	//TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	
	// Level the effects are applied at.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects")
	float ActorLevel = 1.f; 
private:

};
