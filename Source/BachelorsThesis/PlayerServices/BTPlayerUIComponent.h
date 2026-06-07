// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BTCursorMode.h"
#include "BTPlayerUIComponent.generated.h"

struct FBTPromptModel;
class UWidgetInteractionComponent;
class UBTCursorWidget;
class UBTPromptWidget;
class APlayerController;

/**
 * Aggregating service for all interaction-related UI: cursor mode, the rich prompt, and the
 * engine's world-widget interaction. All three change for the same reason ("an interaction is
 * happening"), so they share one component (SRP). Helpers depend only on this service, never on
 * engine UI classes. Owns a UWidgetInteractionComponent used by DI-1. See ARCHITECTURE.md 5.4.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBTPlayerUIComponent();

	/** Switches the on-screen reticle to the given mode. Virtual so tests can capture the call. */
	virtual void SetCursorMode(EBTCursorMode Mode);

	/** Current cursor mode (exposed mainly for tests and integration checks). */
	EBTCursorMode GetCursorMode() const { return CurrentCursorMode; }

	/** Shows the rich prompt populated from the given model (DI-3). Virtual so tests can capture the model. */
	virtual void ShowPrompt(const FBTPromptModel& Model);

	/** Hides the rich prompt. */
	virtual void HidePrompt();

	/** Activates world-widget cursor projection (DI-1). */
	virtual void EnableWidgetInteraction();

	/** Deactivates world-widget cursor projection. */
	virtual void DisableWidgetInteraction();

	/** Simulates a left-click at the widget cursor (used by the F key on screen interactables, DI-1). */
	void ClickWidgetInteraction();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Reticle widget class; defaults to the C++ class, overridable by a Blueprint child for textures. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|UI")
	TSubclassOf<UBTCursorWidget> CursorWidgetClass;

	/** Prompt widget class; defaults to the C++ class, overridable by a Blueprint child for styling. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|UI")
	TSubclassOf<UBTPromptWidget> PromptWidgetClass;

	/** Engine component owned internally; projects a virtual cursor from screen centre onto world widgets. */
	UPROPERTY(Transient)
	TObjectPtr<UWidgetInteractionComponent> WidgetInteraction;

	UPROPERTY(Transient)
	TObjectPtr<UBTCursorWidget> CursorWidget;

	UPROPERTY(Transient)
	TObjectPtr<UBTPromptWidget> PromptWidget;

	EBTCursorMode CurrentCursorMode = EBTCursorMode::Default;

	APlayerController* GetOwningPlayerController() const;
};
