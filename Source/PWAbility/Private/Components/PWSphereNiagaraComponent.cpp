// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Components/PWSphereNiagaraComponent.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"


UPWSphereNiagaraComponent::UPWSphereNiagaraComponent()
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(this);

	BlockComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BlockComponent"));
	BlockComponent->SetupAttachment(this);

	OverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComponent"));
	OverlapComponent->SetupAttachment(this);
}