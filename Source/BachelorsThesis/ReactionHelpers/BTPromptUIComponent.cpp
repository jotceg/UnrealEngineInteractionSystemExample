// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPromptUIComponent.h"

#include "BTInteractionLog.h"
#include "BTInteractor.h"
#include "BTPlayerUIComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"

void UBTPromptUIComponent::RegisterAction(FName ActionId, FText Label, UInputAction* Action, const FBTActionCallback& Callback, bool bIsAllowedInitially)
{
	if (ActionId.IsNone() || !Action)
	{
		UE_LOG(LogInteractionSystem, Warning, TEXT("RegisterAction ignored: invalid ActionId or null Action."));
		return;
	}

	FBTRegisteredAction Registered;
	Registered.ActionId = ActionId;
	Registered.Label = Label;
	Registered.Action = Action;
	Registered.Callback = Callback;
	Registered.bIsAllowed = bIsAllowedInitially;
	RegisteredActions.Add(MoveTemp(Registered));
}

void UBTPromptUIComponent::SetActionAllowed(FName ActionId, bool bAllowed, FText DisallowedReason)
{
	if (FBTRegisteredAction* Registered = FindAction(ActionId))
	{
		Registered->bIsAllowed = bAllowed;
		Registered->DisallowedReason = DisallowedReason;
		RefreshPromptIfVisible();
	}
}

void UBTPromptUIComponent::OnInteractableDetected(IBTInteractor* Interactor)
{
	// Base adds the mapping context.
	Super::OnInteractableDetected(Interactor);

	if (!Interactor)
	{
		return;
	}

	// Route every registered action through the single handler (disambiguated by SourceAction).
	for (const FBTRegisteredAction& Registered : RegisteredActions)
	{
		BindActionTracked(Interactor, Registered.Action, ETriggerEvent::Started, GET_FUNCTION_NAME_CHECKED(UBTPromptUIComponent, OnPromptActionTriggered));
	}

	ActiveUIService = Interactor->GetPlayerUIComponent();
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->ShowPrompt(BuildPromptModel());
	}
}

void UBTPromptUIComponent::OnInteractableLost(IBTInteractor* Interactor)
{
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->HidePrompt();
	}
	ActiveUIService = nullptr;

	// Base unbinds tracked actions and removes the mapping context.
	Super::OnInteractableLost(Interactor);
}

void UBTPromptUIComponent::OnPromptActionTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction)
{
	for (const FBTRegisteredAction& Registered : RegisteredActions)
	{
		if (Registered.Action == SourceAction)
		{
			if (Registered.bIsAllowed)
			{
				Registered.Callback.ExecuteIfBound();
			}
			else
			{
				UE_LOG(LogInteractionSystem, Log, TEXT("Action %s is disallowed; ignored."), *Registered.ActionId.ToString());
			}
			return;
		}
	}
}

FBTRegisteredAction* UBTPromptUIComponent::FindAction(FName ActionId)
{
	return RegisteredActions.FindByPredicate([ActionId](const FBTRegisteredAction& Registered)
	{
		return Registered.ActionId == ActionId;
	});
}

FBTPromptModel UBTPromptUIComponent::BuildPromptModel() const
{
	FBTPromptModel Model;
	Model.ObjectName = ObjectName;
	Model.Options.Reserve(RegisteredActions.Num());

	for (const FBTRegisteredAction& Registered : RegisteredActions)
	{
		FBTPromptOption Option;
		Option.ActionId = Registered.ActionId;
		Option.Label = Registered.Label;
		Option.DisplayKey = GetKeyForAction(Registered.Action);
		Option.bIsAllowed = Registered.bIsAllowed;
		Option.DisallowedReason = Registered.DisallowedReason;
		Model.Options.Add(MoveTemp(Option));
	}

	return Model;
}

void UBTPromptUIComponent::RefreshPromptIfVisible()
{
	if (UBTPlayerUIComponent* UIService = ActiveUIService.Get())
	{
		UIService->ShowPrompt(BuildPromptModel());
	}
}

FKey UBTPromptUIComponent::GetKeyForAction(const UInputAction* Action) const
{
	if (!Action || !MappingContext)
	{
		return FKey();
	}

	for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
	{
		if (Mapping.Action == Action)
		{
			return Mapping.Key;
		}
	}
	return FKey();
}
