// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTScreenSurfaceComponent.h"

#include "BTCursorMode.h"
#include "BTInteractor.h"
#include "BTPlayerUIComponent.h"

void UBTScreenSurfaceComponent::OnInteractableDetected(IBTInteractor* Interactor)
{
	// Base adds the mapping context for the interact key.
	Super::OnInteractableDetected(Interactor);

	if (!Interactor)
	{
		return;
	}

	ActiveUIService = Interactor->GetPlayerUIComponent();
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->SetCursorMode(EBTCursorMode::OnWidget);
		UIService->EnableWidgetInteraction();
	}

	// Started: one click per press, not every frame the key is held.
	BindActionTracked(Interactor, InteractAction, ETriggerEvent::Started,
		GET_FUNCTION_NAME_CHECKED(UBTScreenSurfaceComponent, OnInteractTriggered));
}

void UBTScreenSurfaceComponent::OnInteractableLost(IBTInteractor* Interactor)
{
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->DisableWidgetInteraction();
		UIService->SetCursorMode(EBTCursorMode::Default);
	}
	ActiveUIService = nullptr;

	// Base unbinds the interact action and removes the mapping context.
	Super::OnInteractableLost(Interactor);
}

void UBTScreenSurfaceComponent::OnInteractTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction)
{
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->ClickWidgetInteraction();
	}
}
