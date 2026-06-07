// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BTInteractable.h"
#include "BTInteractableComponent.generated.h"

class IBTInteractor;

/**
 * The framework's component on the object side. Implements IBTInteractable (the dispatcher
 * talks to it) and broadcasts detection events to every IBTInteractableSubscriber component
 * on the same actor. Adding this component to an actor is what makes the actor interactable;
 * its OnRegister also configures the actor's primitives to block the interactable trace
 * channel, so the author cannot forget the collision setup. See ARCHITECTURE.md 3.3.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTInteractableComponent : public UActorComponent, public IBTInteractable
{
	GENERATED_BODY()

public:
	UBTInteractableComponent();

	// --- IBTInteractable ---
	virtual int32 GetMaxDetectionDistance() const override { return MaxDetectionDistance; }
	virtual void OnDetected(IBTInteractor* Interactor) override;
	virtual void OnLost(IBTInteractor* Interactor) override;

protected:
	/** Sets the owner's primitive components to block the dedicated interactable trace channel. */
	virtual void OnRegister() override;

private:
	/** Maximum line-of-sight distance (cm) at which this object registers as detected (FR-1, NFR-4). */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = "1"))
	int32 MaxDetectionDistance = 200;
};
