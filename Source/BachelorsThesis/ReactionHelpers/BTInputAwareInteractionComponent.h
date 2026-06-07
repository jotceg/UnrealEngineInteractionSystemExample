// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "BTInteractionAwareComponent.h"
#include "BTBindingHandle.h"
#include "BTInputAwareInteractionComponent.generated.h"

class IBTInteractor;
class UInputMappingContext;
class UInputAction;

/**
 * Abstract base for reaction helpers that bind keys (DI-2 pickup, DI-3 rich prompt). Adds the
 * per-helper Enhanced Input mapping-context lifecycle on top of UBTInteractionAwareComponent:
 * the mapping context is added on detection and removed on loss, and every action bound through
 * BindActionTracked is unbound automatically on loss. DI-1 (screen) inherits the lifecycle-only
 * base instead, since it uses the cursor-on-widget mechanism, not key input. See ARCHITECTURE.md 4.2.
 */
UCLASS(Abstract, ClassGroup = (Interaction))
class BACHELORSTHESIS_API UBTInputAwareInteractionComponent : public UBTInteractionAwareComponent
{
	GENERATED_BODY()

public:
	/** Mapping context added to the player while this object is detected. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	/** Priority of the mapping context. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Input")
	int32 MappingPriority = 0;

protected:
	/** Adds MappingContext via the interactor's input service. Override and call Super first to add bindings. */
	virtual void OnInteractableDetected(IBTInteractor* Interactor) override;

	/** Unbinds tracked actions and removes MappingContext. Override and call Super last. */
	virtual void OnInteractableLost(IBTInteractor* Interactor) override;

	/** Binds a UFUNCTION on this helper to an action; tracks the handle for automatic unbind on loss. */
	FBTBindingHandle BindActionTracked(IBTInteractor* Interactor, UInputAction* Action, ETriggerEvent Trigger, FName MethodName);

private:
	/** Handles of actions bound since the last detection, released on loss. */
	TArray<FBTBindingHandle> ActiveBindings;
};
