// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BTInteractableSubscriber.generated.h"

class IBTInteractor;

UINTERFACE(MinimalAPI)
class UBTInteractableSubscriber : public UInterface
{
	GENERATED_BODY()
};

/**
 * Lets helper components on the same actor as UBTInteractableComponent receive its
 * detected/lost events through pub/sub, with no hard reference between helper and
 * framework component. UBTInteractableComponent iterates its owner's components and
 * dispatches to every one implementing this interface; merely implementing it registers
 * the helper (no BeginPlay setup, no cached sibling pointer). See ARCHITECTURE.md 3.2.
 */
class BACHELORSTHESIS_API IBTInteractableSubscriber
{
	GENERATED_BODY()

public:
	/** The host's interactable component became detected. */
	virtual void OnHostDetected(IBTInteractor* Interactor) = 0;

	/** The host's interactable component was lost. */
	virtual void OnHostLost(IBTInteractor* Interactor) = 0;
};
