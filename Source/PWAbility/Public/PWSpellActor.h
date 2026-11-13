// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Ability/PWSpellParamsInterface.h"
#include "GameFramework/Actor.h"
#include "PWSpellActor.generated.h"

class USpellParams;
enum EPrecastDecalType : uint8;

UCLASS()
class PWABILITY_API APWSpellActor : public AActor, public IPWSpellParamsInterface
{
	GENERATED_BODY()

public:
	APWSpellActor();
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Projectile|Functions")
	virtual void ResetSpell();
	
	// UFUNCTION()
	// void OnGamePhaseChanged(EGamePhase NewPhase);
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_SpellParams)
	USpellParams* SpellParams;
	UFUNCTION()
	virtual void OnRep_SpellParams();

	UFUNCTION(BlueprintPure, Category = "BaseSpell")
	bool HasReplicatedData() const { return HasAuthority() || bHasReplicatedData; }

	virtual void SetSpellParams_Implementation(USpellParams* Params) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BaseSpell")
	bool bHasReplicatedData = false;

	//Target
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool CheckForCollisionTarget(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool CheckForEffectTarget(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool IsSelf(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool IsEnemy(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool IsAlly(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	virtual bool IsSameCast(const AActor* OtherActor) const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	static bool IsActorProjectile(const AActor* OtherActor);
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	static bool IsNoCollision(const AActor* OtherActor);
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "BaseSpell|Target")
	static bool IsDontTriggerCollision(const AActor* OtherActor);
	// Who an impact here affects: the ability's target resolver decides when the spell params carry one, otherwise only the actor that was hit.
	UFUNCTION(BlueprintCallable, Category = "BaseSpell|Target")
	TArray<AActor*> GetImpactTargets(AActor* HitActor) const;
	

private:

public:
};
