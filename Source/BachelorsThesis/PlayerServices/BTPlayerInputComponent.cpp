// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPlayerInputComponent.h"

#include "BTInteractionLog.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void UBTPlayerInputComponent::AddMappingContext(UInputMappingContext* MappingContext, int32 Priority)
{
	if (!MappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem())
	{
		Subsystem->AddMappingContext(MappingContext, Priority);
	}
	else
	{
		UE_LOG(LogInteractionSystem, Warning, TEXT("AddMappingContext: no Enhanced Input subsystem available."));
	}
}

void UBTPlayerInputComponent::RemoveMappingContext(UInputMappingContext* MappingContext)
{
	if (!MappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem())
	{
		Subsystem->RemoveMappingContext(MappingContext);
	}
}

FBTBindingHandle UBTPlayerInputComponent::BindAction(UInputAction* Action, ETriggerEvent Trigger, UObject* BindingOwner, FName MethodName)
{
	if (!Action || !BindingOwner)
	{
		return FBTBindingHandle();
	}

	UEnhancedInputComponent* EnhancedInput = GetEnhancedInputComponent();
	if (!EnhancedInput)
	{
		UE_LOG(LogInteractionSystem, Warning, TEXT("BindAction: no Enhanced Input component available."));
		return FBTBindingHandle();
	}

	const FEnhancedInputActionEventBinding& Binding = EnhancedInput->BindAction(Action, Trigger, BindingOwner, MethodName);
	return FBTBindingHandle(static_cast<int32>(Binding.GetHandle()));
}

void UBTPlayerInputComponent::UnbindAction(FBTBindingHandle Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInput = GetEnhancedInputComponent())
	{
		EnhancedInput->RemoveBindingByHandle(static_cast<uint32>(Handle.InternalId));
	}
}

UEnhancedInputComponent* UBTPlayerInputComponent::GetEnhancedInputComponent() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Cast<UEnhancedInputComponent>(Pawn->InputComponent);
	}
	return nullptr;
}

UEnhancedInputLocalPlayerSubsystem* UBTPlayerInputComponent::GetInputSubsystem() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return nullptr;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		}
	}
	return nullptr;
}
