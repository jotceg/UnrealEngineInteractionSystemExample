// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTInteractableComponent.h"

#include "BTInteractableSubscriber.h"
#include "BTInteractionChannels.h"
#include "BTInteractionLog.h"
#include "BTPerf.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UBTInteractableComponent::UBTInteractableComponent()
{
	// Detection transitions are event-driven; this component never ticks.
	PrimaryComponentTick.bCanEverTick = false;
}

void UBTInteractableComponent::OnRegister()
{
	Super::OnRegister();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Make every primitive on the owner respond to the interactable trace channel, so the
	// dispatcher's raycast can hit this actor without any per-actor manual collision setup.
	Owner->ForEachComponent<UPrimitiveComponent>(/*bIncludeFromChildActors*/ false,
		[](UPrimitiveComponent* Primitive)
		{
			Primitive->SetCollisionResponseToChannel(BTInteraction::InteractableTraceChannel, ECR_Block);
		});
}

void UBTInteractableComponent::OnDetected(IBTInteractor* Interactor)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UE_LOG(LogInteractionSystem, Log, TEXT("Interactable detected: %s"), *Owner->GetName());

	BT_PERF_SCOPE(STAT_BTSubscriberDispatch, SubscriberDispatch);

	TInlineComponentArray<UActorComponent*> Components(Owner);
	for (UActorComponent* Component : Components)
	{
		if (IBTInteractableSubscriber* Subscriber = Cast<IBTInteractableSubscriber>(Component))
		{
			Subscriber->OnHostDetected(Interactor);
		}
	}
}

void UBTInteractableComponent::OnLost(IBTInteractor* Interactor)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UE_LOG(LogInteractionSystem, Log, TEXT("Interactable lost: %s"), *Owner->GetName());

	BT_PERF_SCOPE(STAT_BTSubscriberDispatch, SubscriberDispatch);

	TInlineComponentArray<UActorComponent*> Components(Owner);
	for (UActorComponent* Component : Components)
	{
		if (IBTInteractableSubscriber* Subscriber = Cast<IBTInteractableSubscriber>(Component))
		{
			Subscriber->OnHostLost(Interactor);
		}
	}
}
