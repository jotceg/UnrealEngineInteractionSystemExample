// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTInputAwareInteractionComponent.h"
#include "BTPickupBehaviorComponent.generated.h"

class IBTInteractor;
class UInputAction;
class UCameraComponent;
class UPrimitiveComponent;
class UBTPlayerUIComponent;
struct FInputActionValue;

/**
 * Concrete reaction helper for the pickup variant (DI-2). Implements the grab/carry/drop/throw
 * state machine for a free-physics object: on detection it changes the cursor and binds the
 * three actions; grab attaches the mesh in front of the camera (physics off), drop restores
 * physics, throw restores physics and adds an impulse along the view. Carrying uses attachment
 * rather than a per-tick physics handle, so no tick is needed (NFR-3). See ARCHITECTURE.md 4.4.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTPickupBehaviorComponent : public UBTInputAwareInteractionComponent
{
	GENERATED_BODY()

public:
	/** Single grab/drop toggle key (F): grabs when hovered, drops when carried. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Pickup")
	TObjectPtr<UInputAction> GrabAction;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Pickup")
	TObjectPtr<UInputAction> ThrowAction;

	/** Velocity change (cm/s) applied along the view direction on throw. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Pickup")
	float ThrowImpulse = 250.f;

	/** Offset from the camera at which the carried object is held. Centred on the view axis so the
	 *  held object occupies the crosshair and stays the only detected interactable while carried. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Pickup")
	FVector HandOffset = FVector(45.f, 0.f, 0.f);

protected:
	virtual void OnInteractableDetected(IBTInteractor* Interactor) override;
	virtual void OnInteractableLost(IBTInteractor* Interactor) override;

	UFUNCTION()
	void OnGrabTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction);

	UFUNCTION()
	void OnThrowTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction);

private:
	enum class EState : uint8
	{
		Idle,
		Hovered,
		Carried
	};

	EState CurrentState = EState::Idle;

	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> CarriedPrimitive;

	TWeakObjectPtr<UCameraComponent> ActiveCamera;
	TWeakObjectPtr<UBTPlayerUIComponent> ActiveUIService;

	UPrimitiveComponent* ResolveCarriedPrimitive();

	/** Detaches the carried object, restores physics, and optionally throws it along the view. */
	void ReleaseCarried(bool bThrow);
};
