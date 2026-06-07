// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BTInteractableSubscriber.h"
#include "BTInteractionAwareComponent.generated.h"

class IBTInteractor;

/**
 * Abstract base for any reaction helper that wants to react to its host's detection lifecycle.
 * Implements IBTInteractableSubscriber and seals the interface methods (final), forwarding to
 * protected virtuals so concrete helpers always override OnInteractableDetected/Lost and never
 * the interface methods directly (uniform contract). No BeginPlay lookup, no cached sibling,
 * no unsubscribe: merely existing on the actor registers the helper. See ARCHITECTURE.md 4.1.
 */
UCLASS(Abstract, ClassGroup = (Interaction))
class BACHELORSTHESIS_API UBTInteractionAwareComponent : public UActorComponent, public IBTInteractableSubscriber
{
	GENERATED_BODY()

public:
	// --- IBTInteractableSubscriber (sealed; forward to protected virtuals) ---
	virtual void OnHostDetected(IBTInteractor* Interactor) override final { OnInteractableDetected(Interactor); }
	virtual void OnHostLost(IBTInteractor* Interactor) override final { OnInteractableLost(Interactor); }

protected:
	/** The host became detected. Override to react. */
	virtual void OnInteractableDetected(IBTInteractor* Interactor) {}

	/** The host was lost. Override to react. */
	virtual void OnInteractableLost(IBTInteractor* Interactor) {}
};
