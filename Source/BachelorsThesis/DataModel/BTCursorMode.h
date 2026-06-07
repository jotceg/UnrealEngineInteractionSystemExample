// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTCursorMode.generated.h"

/**
 * Discrete cursor states the player UI service can switch between.
 * Each reaction helper sets the mode on detection and resets to Default on loss;
 * the UI service does not need to know which helper requested a given mode.
 * Extend by adding entries (NFR-1, open-closed).
 */
UENUM(BlueprintType)
enum class EBTCursorMode : uint8
{
	/** Crosshair, no interactable detected. */
	Default,

	/** Pickup detected. Used by UBTPickupBehaviorComponent. */
	GrabHand,

	/** Screen detected. Used by UBTScreenSurfaceComponent. */
	OnWidget
};
