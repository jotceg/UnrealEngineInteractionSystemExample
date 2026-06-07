// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

/**
 * Dedicated log category for the interaction system.
 * Verbose: per-trace detection details (off by default). Log: detection state transitions.
 * Warning: recoverable misconfiguration. Error: genuine failures.
 */
BACHELORSTHESIS_API DECLARE_LOG_CATEGORY_EXTERN(LogInteractionSystem, Log, All);
