// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BTInteractableSubscriber.h"
#include "BTTestSubscriberComponent.generated.h"

class IBTInteractor;

/**
 * Test-only subscriber that counts the detection events it receives. Used by the automation
 * tests to verify UBTInteractableComponent's pub/sub dispatch without involving the player or
 * the asynchronous raycast.
 */
UCLASS(NotBlueprintable, HideDropdown)
class UBTTestSubscriberComponent : public UActorComponent, public IBTInteractableSubscriber
{
	GENERATED_BODY()

public:
	int32 DetectedCount = 0;
	int32 LostCount = 0;

	/** Optional label, recorded into Events so transition-order tests can assert across subscribers. */
	FString Label;

	/** Shared, ordered log of events across all test subscribers (e.g. "A:Detected"). Cleared per test. */
	static TArray<FString> Events;

	virtual void OnHostDetected(IBTInteractor* /*Interactor*/) override
	{
		++DetectedCount;
		Events.Add(Label + TEXT(":Detected"));
	}

	virtual void OnHostLost(IBTInteractor* /*Interactor*/) override
	{
		++LostCount;
		Events.Add(Label + TEXT(":Lost"));
	}
};
