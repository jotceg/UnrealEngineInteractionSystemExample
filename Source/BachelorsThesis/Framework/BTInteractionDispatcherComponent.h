// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "WorldCollision.h"
#include "BTInteractor.h"
#include "BTInteractionChannels.h"
#include "BTInteractionDispatcherComponent.generated.h"

class UCameraComponent;
class UBTPlayerInputComponent;
class UBTPlayerUIComponent;

/**
 * The framework's component on the player side. Performs the per-cycle detection raycast from
 * the camera and dispatches OnDetected/OnLost to whichever interactable is currently aimed at.
 * Implements IBTInteractor so it can be handed to interactables and their reaction helpers.
 *
 * Uses an asynchronous line trace with callback-recursive re-arm: the first trace fires in
 * BeginPlay, and each completed trace processes its result then immediately requests the next.
 * See ARCHITECTURE.md 5.2 and decision #2.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTInteractionDispatcherComponent : public UActorComponent, public IBTInteractor
{
	GENERATED_BODY()

public:
	UBTInteractionDispatcherComponent();

	/** Pure FR-1 acceptance predicate: a hit counts as detected only within the type's max distance. */
	static bool IsWithinDetectionDistance(int32 MaxDistance, float HitDistance);

	// --- IBTInteractor ---
	virtual UCameraComponent* GetInteractorCamera() const override;
	virtual UBTPlayerInputComponent* GetPlayerInputComponent() const override;
	virtual UBTPlayerUIComponent* GetPlayerUIComponent() const override;
	virtual ACharacter* GetInteractorCharacter() const override;

#if WITH_AUTOMATION_TESTS
	/** Test-only seam: drive the selection transition directly, without arming the async trace. */
	void TestSetCurrentInteractable(UActorComponent* NewInteractable) { SetCurrentInteractable(NewInteractable); }
#endif

protected:
	virtual void BeginPlay() override;

private:
	/** Outer bound of the trace (cm); per-interactable GetMaxDetectionDistance filters within this. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = "1"))
	float MaxTraceDistance = 500.f;

	/** Dedicated collision channel the trace runs on; defaults to the shared interactable channel. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractableTraceChannel = BTInteraction::InteractableTraceChannel;

	/** Currently selected interactable component; weak so a destroyed actor is detected as staleness. */
	UPROPERTY(Transient)
	TWeakObjectPtr<UActorComponent> CurrentInteractable;

	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> CachedCamera;

	UPROPERTY(Transient)
	TObjectPtr<UBTPlayerInputComponent> CachedInputService;

	UPROPERTY(Transient)
	TObjectPtr<UBTPlayerUIComponent> CachedUIService;

	FTraceHandle TraceHandle;
	FTraceDelegate TraceDelegate;

	/** Arms the next asynchronous trace from the camera along the view direction. */
	void RequestTrace();

	/** Processes the completed trace, updates the selected interactable, then re-arms. */
	void OnTraceCompleted(const FTraceHandle& Handle, FTraceDatum& Datum);

	/** Transitions selection: fires OnLost on the previous and OnDetected on the new interactable. */
	void SetCurrentInteractable(UActorComponent* NewInteractable);
};
