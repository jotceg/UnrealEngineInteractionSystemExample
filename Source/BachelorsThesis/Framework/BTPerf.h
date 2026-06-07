// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"

/**
 * Performance instrumentation for the interaction system (NFR-3).
 *
 * `BT_PERF_SCOPE(StatId, Name)` wraps a hot path. It always emits a UE cycle stat (visible under
 * `stat BTInteraction` and in Unreal Insights, with effectively zero cost when not profiling) and,
 * when `BT_PERF_LOG` is set to 1, additionally logs the scope duration in microseconds to
 * `LogInteractionSystem` (Verbose). `BT_PERF_LOG` defaults to 0 so normal play performs no per-tick
 * string formatting (honouring NFR-3, CODING_STANDARDS); flip it to 1 for an explicit measurement
 * run, read the numbers, then flip it back.
 */
DECLARE_STATS_GROUP(TEXT("BTInteraction"), STATGROUP_BTInteraction, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Dispatcher trace callback"), STAT_BTTraceCallback, STATGROUP_BTInteraction, BACHELORSTHESIS_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Interactable subscriber dispatch"), STAT_BTSubscriberDispatch, STATGROUP_BTInteraction, BACHELORSTHESIS_API);

#ifndef BT_PERF_LOG
#define BT_PERF_LOG 0
#endif

#if BT_PERF_LOG

#include "BTInteractionLog.h"
#include "HAL/PlatformTime.h"

/** RAII timer that logs the duration of its scope in microseconds when destroyed. */
struct FBTScopedPerfLogger
{
	const TCHAR* Label;
	double StartSeconds;

	explicit FBTScopedPerfLogger(const TCHAR* InLabel)
		: Label(InLabel)
		, StartSeconds(FPlatformTime::Seconds())
	{
	}

	~FBTScopedPerfLogger()
	{
		const double Micros = (FPlatformTime::Seconds() - StartSeconds) * 1000000.0;
		UE_LOG(LogInteractionSystem, Verbose, TEXT("[perf] %s: %.2f us"), Label, Micros);
	}
};

#define BT_PERF_LOG_SCOPE(Name) const FBTScopedPerfLogger BTPerfLogger_##Name(TEXT(#Name))

#else

#define BT_PERF_LOG_SCOPE(Name)

#endif // BT_PERF_LOG

/** Wrap a hot path: cycle stat (always) + optional microsecond log (when BT_PERF_LOG == 1). */
#define BT_PERF_SCOPE(StatId, Name) \
	SCOPE_CYCLE_COUNTER(StatId); \
	BT_PERF_LOG_SCOPE(Name)
