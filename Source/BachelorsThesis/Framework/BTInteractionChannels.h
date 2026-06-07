// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

/**
 * Shared, single-source-of-truth constant for the dedicated interactable trace channel.
 * Mirrors the named "Interactable" channel declared in Config/DefaultEngine.ini.
 *
 * ECC_GameTraceChannel1 is already reserved by the template's "Projectile" collision
 * profile, so the interaction system uses ECC_GameTraceChannel2 (see DISCOVERED_GAPS.md
 * Gap #5). Both the dispatcher's trace and the interactable component's collision setup
 * reference this constant so they can never drift apart.
 */
namespace BTInteraction
{
	inline constexpr ECollisionChannel InteractableTraceChannel = ECC_GameTraceChannel2;
}
