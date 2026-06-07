// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Callback a host actor provides when registering an action with UBTPromptUIComponent (DI-3).
 * Invoked when the player triggers the action and the action is currently allowed.
 *
 * A non-dynamic delegate is used deliberately: actions are registered from C++ in the host
 * actor's BeginPlay (Blueprint-only implementations of the framework are out of scope per
 * ARCHITECTURE.md decision #7), so the lighter-weight delegate keeps the call path direct.
 */
DECLARE_DELEGATE(FBTActionCallback);
