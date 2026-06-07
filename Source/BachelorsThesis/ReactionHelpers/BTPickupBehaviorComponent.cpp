// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPickupBehaviorComponent.h"

#include "BTCursorMode.h"
#include "BTInteractionLog.h"
#include "BTInteractor.h"
#include "BTPlayerUIComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

void UBTPickupBehaviorComponent::OnInteractableDetected(IBTInteractor* Interactor)
{
	// While carrying, the held object occupies the crosshair and stays the current interactable,
	// so this should not re-fire. Guard anyway: a stray re-detection must never re-bind the actions
	// (duplicate bindings made one keypress drop-and-regrab) or reset the carry state.
	if (CurrentState == EState::Carried)
	{
		return;
	}

	// Base adds the mapping context.
	Super::OnInteractableDetected(Interactor);

	if (!Interactor)
	{
		return;
	}

	ActiveCamera = Interactor->GetInteractorCamera();
	ActiveUIService = Interactor->GetPlayerUIComponent();

	// Bind on Started (fires once per press) rather than Triggered (every frame held).
	BindActionTracked(Interactor, GrabAction, ETriggerEvent::Started, GET_FUNCTION_NAME_CHECKED(UBTPickupBehaviorComponent, OnGrabTriggered));
	BindActionTracked(Interactor, ThrowAction, ETriggerEvent::Started, GET_FUNCTION_NAME_CHECKED(UBTPickupBehaviorComponent, OnThrowTriggered));

	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->SetCursorMode(EBTCursorMode::GrabHand);
	}
	CurrentState = EState::Hovered;
}

void UBTPickupBehaviorComponent::OnInteractableLost(IBTInteractor* Interactor)
{
	// Reset the cursor whenever the player stops aiming at the pickup, even while carrying it,
	// so the grab-hand cursor never gets stuck after looking away mid-carry and then throwing.
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->SetCursorMode(EBTCursorMode::Default);
	}

	// While carried the object stays held even if the player looks away, so keep input and state.
	if (CurrentState == EState::Carried)
	{
		return;
	}

	CurrentState = EState::Idle;

	// Base unbinds tracked actions and removes the mapping context.
	Super::OnInteractableLost(Interactor);
}

void UBTPickupBehaviorComponent::OnGrabTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction)
{
	// Single key (F) toggles: drop if already carrying, otherwise grab when hovered.
	if (CurrentState == EState::Carried)
	{
		ReleaseCarried(/*bThrow*/ false);
		return;
	}

	if (CurrentState != EState::Hovered)
	{
		return;
	}

	UCameraComponent* Camera = ActiveCamera.Get();
	UPrimitiveComponent* Primitive = ResolveCarriedPrimitive();
	if (!Camera || !Primitive)
	{
		return;
	}

	Primitive->SetSimulatePhysics(false);
	// While carried the object is attached in front of the camera; let it ignore the Pawn channel
	// so it cannot collide with the carrier's capsule and lock the character in place.
	Primitive->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Primitive->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Primitive->SetRelativeLocation(HandOffset);
	Primitive->SetRelativeRotation(FRotator::ZeroRotator);
	CurrentState = EState::Carried;
}

void UBTPickupBehaviorComponent::OnThrowTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction)
{
	if (CurrentState == EState::Carried)
	{
		ReleaseCarried(/*bThrow*/ true);
	}
}

void UBTPickupBehaviorComponent::ReleaseCarried(bool bThrow)
{
	UPrimitiveComponent* Primitive = CarriedPrimitive;
	if (!Primitive)
	{
		return;
	}

	Primitive->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	// Restore blocking against the Pawn channel now that the object is free in the world again.
	Primitive->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Primitive->SetSimulatePhysics(true);
	// Continuous collision detection prevents the object from tunnelling through the floor.
	Primitive->SetUseCCD(true);
	// Clear any residual velocity carried over from following the camera, so a plain drop
	// falls straight down instead of being flung (and never punches through the floor).
	Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Primitive->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	if (bThrow)
	{
		if (UCameraComponent* Camera = ActiveCamera.Get())
		{
			// bVelChange = true: ThrowImpulse is a mass-independent velocity change.
			Primitive->AddImpulse(Camera->GetForwardVector() * ThrowImpulse, NAME_None, /*bVelChange*/ true);
		}
	}

	// Still likely aiming at the object right after release.
	CurrentState = EState::Hovered;
	CarriedPrimitive = nullptr;
}

UPrimitiveComponent* UBTPickupBehaviorComponent::ResolveCarriedPrimitive()
{
	if (CarriedPrimitive)
	{
		return CarriedPrimitive;
	}

	if (const AActor* Owner = GetOwner())
	{
		if (UStaticMeshComponent* Mesh = Owner->FindComponentByClass<UStaticMeshComponent>())
		{
			CarriedPrimitive = Mesh;
		}
	}
	return CarriedPrimitive;
}
