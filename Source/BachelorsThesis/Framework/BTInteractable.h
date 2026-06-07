// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BTInteractable.generated.h"

class IBTInteractor;

UINTERFACE(MinimalAPI)
class UBTInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contract every interactable object must expose so the dispatcher can talk to it
 * polymorphically. Deliberately minimal (FR-3): the framework only needs to ask how far
 * the object is detectable and to notify it when its detected/lost state changes.
 * Reaction semantics (input, prompts, validation) are kept out of this boundary on
 * purpose (see DISCOVERED_GAPS.md Gap #2).
 */
class BACHELORSTHESIS_API IBTInteractable
{
	GENERATED_BODY()

public:
	/** Maximum line-of-sight distance (cm) at which this object still registers as detected (FR-1). */
	virtual int32 GetMaxDetectionDistance() const = 0;

	/** Called by the dispatcher when this object becomes the currently aimed-at interactable. */
	virtual void OnDetected(IBTInteractor* Interactor) = 0;

	/** Called by the dispatcher when this object stops being the currently aimed-at interactable. */
	virtual void OnLost(IBTInteractor* Interactor) = 0;
};
