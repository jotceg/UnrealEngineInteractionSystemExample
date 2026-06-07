// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputTriggers.h"
#include "BTBindingHandle.h"
#include "BTPlayerInputComponent.generated.h"

class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * Aggregating service that wraps the Enhanced Input lifecycle so reaction helpers never
 * depend on engine-specific input classes. Lives on the same character as the dispatcher.
 * Helpers call this on detection transitions (not per tick), so the internal subsystem and
 * input-component lookups have negligible cost. See ARCHITECTURE.md 5.3 and Gap #1 symmetry.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Adds a mapping context to the local player's Enhanced Input subsystem. */
	void AddMappingContext(UInputMappingContext* MappingContext, int32 Priority);

	/** Removes a previously added mapping context. */
	void RemoveMappingContext(UInputMappingContext* MappingContext);

	/** Binds a UObject method to an input action and trigger. Returns a handle for later unbinding. */
	FBTBindingHandle BindAction(UInputAction* Action, ETriggerEvent Trigger, UObject* BindingOwner, FName MethodName);

	/** Removes a binding previously created by BindAction. */
	void UnbindAction(FBTBindingHandle Handle);

private:
	/** The owning pawn's Enhanced Input component, or nullptr if input is not set up yet. */
	UEnhancedInputComponent* GetEnhancedInputComponent() const;

	/** The local player's Enhanced Input subsystem, or nullptr if unavailable. */
	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;
};
