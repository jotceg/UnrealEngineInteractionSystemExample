// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTInputAwareInteractionComponent.h"
#include "BTActionCallback.h"
#include "BTPromptModel.h"
#include "BTPromptUIComponent.generated.h"

class IBTInteractor;
class UInputAction;
class UBTPlayerUIComponent;
struct FInputActionValue;

/**
 * One action a rich-prompt host supports: identity, label, the input action that triggers it,
 * the host's callback, and the current validation state. Validation lives on the host side
 * (DI-3): the host flips bIsAllowed via SetActionAllowed when its internal state changes.
 */
USTRUCT()
struct FBTRegisteredAction
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActionId;

	UPROPERTY()
	FText Label;

	UPROPERTY()
	TObjectPtr<UInputAction> Action = nullptr;

	UPROPERTY()
	bool bIsAllowed = true;

	UPROPERTY()
	FText DisallowedReason;

	/** Host-supplied callback, invoked on trigger only when bIsAllowed. Non-reflected (delegate). */
	FBTActionCallback Callback;
};

/**
 * Concrete reaction helper for the rich-prompt variant (DI-3). Holds a registry of named actions,
 * shows a data-driven FBTPromptModel while detected, and routes key presses to the registered
 * callbacks, honouring per-action validation. The host stays thin: it calls RegisterAction in
 * BeginPlay and SetActionAllowed when its state changes. See ARCHITECTURE.md 4.3.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTPromptUIComponent : public UBTInputAwareInteractionComponent
{
	GENERATED_BODY()

public:
	/** Declares an action this object supports. Called by the host actor in its BeginPlay. */
	void RegisterAction(FName ActionId, FText Label, UInputAction* Action, const FBTActionCallback& Callback, bool bIsAllowedInitially = true);

	/** Updates an action's validation state at runtime (e.g. wardrobe becomes unlocked). */
	void SetActionAllowed(FName ActionId, bool bAllowed, FText DisallowedReason = FText::GetEmpty());

	/** Display name shown in the prompt header. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Prompt")
	FText ObjectName;

protected:
	virtual void OnInteractableDetected(IBTInteractor* Interactor) override;
	virtual void OnInteractableLost(IBTInteractor* Interactor) override;

	/** Single routing handler: matches SourceAction to a registered action and runs its callback if allowed. */
	UFUNCTION()
	void OnPromptActionTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction);

private:
	UPROPERTY()
	TArray<FBTRegisteredAction> RegisteredActions;

	/** UI service of the player currently aiming at this object; valid only between detect and loss. */
	UPROPERTY(Transient)
	TWeakObjectPtr<UBTPlayerUIComponent> ActiveUIService;

	FBTRegisteredAction* FindAction(FName ActionId);
	FBTPromptModel BuildPromptModel() const;
	void RefreshPromptIfVisible();
	FKey GetKeyForAction(const UInputAction* Action) const;
};
