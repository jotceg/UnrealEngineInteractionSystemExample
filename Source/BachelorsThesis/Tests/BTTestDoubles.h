// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputActionValue.h"
#include "BTInteractor.h"
#include "BTCursorMode.h"
#include "BTPromptModel.h"
#include "BTPlayerUIComponent.h"
#include "BTPromptUIComponent.h"
#include "BTScreenSurfaceComponent.h"
#include "BTPickupBehaviorComponent.h"
#include "BTTestDoubles.generated.h"

class UCameraComponent;
class UBTPlayerInputComponent;
class UInputAction;
class ACharacter;

/**
 * Test-only IBTInteractor whose getters return whatever the test sets. Lets reaction helpers be
 * exercised without a real player pawn (helpers depend only on the IBTInteractor getters).
 */
UCLASS(NotBlueprintable, HideDropdown)
class UBTMockInteractor : public UObject, public IBTInteractor
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient) TObjectPtr<UCameraComponent> MockCamera;
	UPROPERTY(Transient) TObjectPtr<UBTPlayerInputComponent> MockInput;
	UPROPERTY(Transient) TObjectPtr<UBTPlayerUIComponent> MockUI;
	UPROPERTY(Transient) TObjectPtr<ACharacter> MockCharacter;

	virtual UCameraComponent* GetInteractorCamera() const override { return MockCamera; }
	virtual UBTPlayerInputComponent* GetPlayerInputComponent() const override { return MockInput; }
	virtual UBTPlayerUIComponent* GetPlayerUIComponent() const override { return MockUI; }
	virtual ACharacter* GetInteractorCharacter() const override { return MockCharacter; }
};

/**
 * Test-only UI service that records calls instead of touching real widgets, so helper reaction
 * paths can be asserted (cursor mode, prompt model, widget-interaction toggling).
 */
UCLASS(NotBlueprintable, HideDropdown)
class UBTTestUIComponent : public UBTPlayerUIComponent
{
	GENERATED_BODY()

public:
	EBTCursorMode LastCursorMode = EBTCursorMode::Default;
	int32 CursorSetCount = 0;
	bool bWidgetEnabled = false;
	int32 EnableCount = 0;
	int32 DisableCount = 0;
	bool bPromptShown = false;
	int32 ShowCount = 0;
	int32 HideCount = 0;
	FBTPromptModel CapturedModel;

	virtual void SetCursorMode(EBTCursorMode Mode) override { LastCursorMode = Mode; ++CursorSetCount; }
	virtual void ShowPrompt(const FBTPromptModel& Model) override { CapturedModel = Model; bPromptShown = true; ++ShowCount; }
	virtual void HidePrompt() override { bPromptShown = false; ++HideCount; }
	virtual void EnableWidgetInteraction() override { bWidgetEnabled = true; ++EnableCount; }
	virtual void DisableWidgetInteraction() override { bWidgetEnabled = false; ++DisableCount; }
};

/** Exposes the prompt helper's protected reaction entry points + routing handler to tests. */
UCLASS(NotBlueprintable, HideDropdown)
class UBTTestPromptComponent : public UBTPromptUIComponent
{
	GENERATED_BODY()

public:
	void TestDetect(IBTInteractor* Interactor) { OnInteractableDetected(Interactor); }
	void TestLost(IBTInteractor* Interactor) { OnInteractableLost(Interactor); }
	void TestTrigger(const UInputAction* Action) { OnPromptActionTriggered(FInputActionValue(), 0.f, 0.f, Action); }
};

/** Exposes the screen helper's protected reaction entry points to tests. */
UCLASS(NotBlueprintable, HideDropdown)
class UBTTestScreenComponent : public UBTScreenSurfaceComponent
{
	GENERATED_BODY()

public:
	void TestDetect(IBTInteractor* Interactor) { OnInteractableDetected(Interactor); }
	void TestLost(IBTInteractor* Interactor) { OnInteractableLost(Interactor); }
};

/** Exposes the pickup helper's protected reaction entry points to tests. */
UCLASS(NotBlueprintable, HideDropdown)
class UBTTestPickupComponent : public UBTPickupBehaviorComponent
{
	GENERATED_BODY()

public:
	void TestDetect(IBTInteractor* Interactor) { OnInteractableDetected(Interactor); }
	void TestLost(IBTInteractor* Interactor) { OnInteractableLost(Interactor); }
};
