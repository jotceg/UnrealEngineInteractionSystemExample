// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BTInteractor.generated.h"

class UCameraComponent;
class UBTPlayerInputComponent;
class UBTPlayerUIComponent;
class ACharacter;

UINTERFACE(MinimalAPI)
class UBTInteractor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contract the dispatcher exposes to anything that needs to talk to "the player" - mainly
 * reaction helper components on the object side. Wraps engine-specific player resources
 * behind project-owned services so helpers never hard-reference engine input/UI classes.
 * Player-side mirror of IBTInteractable. See ARCHITECTURE.md 5.1.
 */
class BACHELORSTHESIS_API IBTInteractor
{
	GENERATED_BODY()

public:
	/** Camera used as the raycast origin and for pickup hand-position math. */
	virtual UCameraComponent* GetInteractorCamera() const = 0;

	/** Project-owned Enhanced Input service (shields helpers from engine input classes). */
	virtual UBTPlayerInputComponent* GetPlayerInputComponent() const = 0;

	/** Project-owned interaction UI service (cursor, prompt, world-widget interaction). */
	virtual UBTPlayerUIComponent* GetPlayerUIComponent() const = 0;

	/** The interacting character (always ACharacter in this project; FPS Template base). */
	virtual ACharacter* GetInteractorCharacter() const = 0;
};
