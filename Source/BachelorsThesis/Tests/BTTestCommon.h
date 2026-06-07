// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

/**
 * Shared automation-test flags for the interaction-system tests. Defined once as an inline variable
 * so multiple test .cpp files can use it without colliding under a unity build (an anonymous-namespace
 * constant duplicated per file would redefine in the merged unity translation unit).
 */
inline constexpr EAutomationTestFlags BTTestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
