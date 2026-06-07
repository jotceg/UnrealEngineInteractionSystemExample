// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPickupInteractableActor.h"

#include "BTHighlightComponent.h"
#include "BTInteractableComponent.h"
#include "BTPickupBehaviorComponent.h"
#include "Components/StaticMeshComponent.h"

ABTPickupInteractableActor::ABTPickupInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;
	PickupMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	PickupMesh->SetSimulatePhysics(true);

	Interactable = CreateDefaultSubobject<UBTInteractableComponent>(TEXT("Interactable"));
	PickupBehavior = CreateDefaultSubobject<UBTPickupBehaviorComponent>(TEXT("PickupBehavior"));
	Highlight = CreateDefaultSubobject<UBTHighlightComponent>(TEXT("Highlight"));
}
