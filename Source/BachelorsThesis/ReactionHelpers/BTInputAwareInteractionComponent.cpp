// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTInputAwareInteractionComponent.h"

#include "BTInteractor.h"
#include "BTPlayerInputComponent.h"

void UBTInputAwareInteractionComponent::OnInteractableDetected(IBTInteractor* Interactor)
{
	if (!Interactor || !MappingContext)
	{
		return;
	}

	if (UBTPlayerInputComponent* InputService = Interactor->GetPlayerInputComponent())
	{
		InputService->AddMappingContext(MappingContext, MappingPriority);
	}
}

void UBTInputAwareInteractionComponent::OnInteractableLost(IBTInteractor* Interactor)
{
	if (Interactor)
	{
		if (UBTPlayerInputComponent* InputService = Interactor->GetPlayerInputComponent())
		{
			for (const FBTBindingHandle& Handle : ActiveBindings)
			{
				InputService->UnbindAction(Handle);
			}

			if (MappingContext)
			{
				InputService->RemoveMappingContext(MappingContext);
			}
		}
	}

	ActiveBindings.Reset();
}

FBTBindingHandle UBTInputAwareInteractionComponent::BindActionTracked(IBTInteractor* Interactor, UInputAction* Action, ETriggerEvent Trigger, FName MethodName)
{
	if (!Interactor)
	{
		return FBTBindingHandle();
	}

	UBTPlayerInputComponent* InputService = Interactor->GetPlayerInputComponent();
	if (!InputService)
	{
		return FBTBindingHandle();
	}

	const FBTBindingHandle Handle = InputService->BindAction(Action, Trigger, this, MethodName);
	if (Handle.IsValid())
	{
		ActiveBindings.Add(Handle);
	}
	return Handle;
}
