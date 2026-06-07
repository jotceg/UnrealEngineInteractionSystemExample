// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPlayerUIComponent.h"

#include "BTCursorWidget.h"
#include "BTInteractionLog.h"
#include "BTPromptModel.h"
#include "BTPromptWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "InputCoreTypes.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UBTPlayerUIComponent::UBTPlayerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Default to the C++ widget classes so the system works without any Blueprint setup;
	// a Blueprint-derived class can override either to supply textures, fonts, and colours.
	CursorWidgetClass = UBTCursorWidget::StaticClass();
	PromptWidgetClass = UBTPromptWidget::StaticClass();
}

void UBTPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	APlayerController* PlayerController = GetOwningPlayerController();

	// Create and show the reticle.
	if (PlayerController && CursorWidgetClass)
	{
		CursorWidget = CreateWidget<UBTCursorWidget>(PlayerController, CursorWidgetClass);
		if (CursorWidget)
		{
			CursorWidget->AddToViewport();
			CursorWidget->ApplyCursorMode(CurrentCursorMode);
		}
	}

	// Own the world-widget interaction component, projecting from screen centre along the view.
	if (Owner)
	{
		WidgetInteraction = NewObject<UWidgetInteractionComponent>(Owner, TEXT("BTWidgetInteraction"));
		if (WidgetInteraction)
		{
			if (UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>())
			{
				WidgetInteraction->SetupAttachment(Camera);
			}
			WidgetInteraction->InteractionSource = EWidgetInteractionSource::CenterScreen;
			WidgetInteraction->bShowDebug = false;
			WidgetInteraction->RegisterComponent();
			WidgetInteraction->Deactivate();
		}
	}
}

void UBTPlayerUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CursorWidget)
	{
		CursorWidget->RemoveFromParent();
		CursorWidget = nullptr;
	}
	if (PromptWidget)
	{
		PromptWidget->RemoveFromParent();
		PromptWidget = nullptr;
	}
	if (WidgetInteraction)
	{
		WidgetInteraction->DestroyComponent();
		WidgetInteraction = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UBTPlayerUIComponent::SetCursorMode(EBTCursorMode Mode)
{
	CurrentCursorMode = Mode;
	if (CursorWidget)
	{
		CursorWidget->ApplyCursorMode(Mode);
	}
	UE_LOG(LogInteractionSystem, Verbose, TEXT("Cursor mode set to %d"), static_cast<int32>(Mode));
}

void UBTPlayerUIComponent::ShowPrompt(const FBTPromptModel& Model)
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController || !PromptWidgetClass)
	{
		return;
	}

	// Created on demand and reused thereafter (no widget exists when no interaction is active).
	if (!PromptWidget)
	{
		PromptWidget = CreateWidget<UBTPromptWidget>(PlayerController, PromptWidgetClass);
	}
	if (PromptWidget)
	{
		PromptWidget->SetModel(Model);
		if (!PromptWidget->IsInViewport())
		{
			PromptWidget->AddToViewport();
		}
		PromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UBTPlayerUIComponent::HidePrompt()
{
	if (PromptWidget)
	{
		PromptWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBTPlayerUIComponent::EnableWidgetInteraction()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->Activate();
	}
}

void UBTPlayerUIComponent::DisableWidgetInteraction()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->Deactivate();
	}
}

void UBTPlayerUIComponent::ClickWidgetInteraction()
{
	if (WidgetInteraction)
	{
		// Press + release a virtual left mouse button where the widget cursor points,
		// so pressing F triggers the button currently under the crosshair (DI-1).
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

APlayerController* UBTPlayerUIComponent::GetOwningPlayerController() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Cast<APlayerController>(Pawn->GetController());
	}
	return nullptr;
}
