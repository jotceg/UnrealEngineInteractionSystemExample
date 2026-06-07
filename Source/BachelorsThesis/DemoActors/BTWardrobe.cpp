// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTWardrobe.h"

#include "BTActionCallback.h"
#include "BTInteractableComponent.h"
#include "BTPromptUIComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"

namespace
{
	const FName OpenActionId(TEXT("Open"));
}

ABTWardrobe::ABTWardrobe()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(BodyMesh);

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));

	Interactable = CreateDefaultSubobject<UBTInteractableComponent>(TEXT("Interactable"));
	Prompt = CreateDefaultSubobject<UBTPromptUIComponent>(TEXT("Prompt"));
}

void ABTWardrobe::BeginPlay()
{
	Super::BeginPlay();

	ClosedLocation = DoorMesh->GetRelativeLocation();

	if (DoorCurve)
	{
		FOnTimelineFloat ProgressDelegate;
		ProgressDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ABTWardrobe, HandleDoorProgress));
		DoorTimeline->AddInterpFloat(DoorCurve, ProgressDelegate);
	}

	FOnTimelineEvent FinishedDelegate;
	FinishedDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ABTWardrobe, OnSlideFinished));
	DoorTimeline->SetTimelineFinishedFunc(FinishedDelegate);

	Prompt->ObjectName = DisplayName;

	FBTActionCallback OpenCallback;
	OpenCallback.BindUObject(this, &ABTWardrobe::OnOpenActionExecuted);
	Prompt->RegisterAction(OpenActionId, OpenLabel, OpenAction, OpenCallback, /*bIsAllowedInitially*/ true);

	if (bIsLocked)
	{
		Prompt->SetActionAllowed(OpenActionId, false, LockedReason);
	}
}

void ABTWardrobe::OnOpenActionExecuted()
{
	// Toggle open/close; ignore presses mid-slide so the animation cannot be restarted.
	if (State == EWardrobeState::Closed)
	{
		State = EWardrobeState::Opening;
		DoorTimeline->PlayFromStart();
		Prompt->SetActionAllowed(OpenActionId, false, OpenedReason);
	}
	else if (State == EWardrobeState::Open)
	{
		State = EWardrobeState::Closing;
		DoorTimeline->Reverse();
		Prompt->SetActionAllowed(OpenActionId, false, OpenedReason);
	}
}

void ABTWardrobe::OnSlideFinished()
{
	if (State == EWardrobeState::Opening)
	{
		State = EWardrobeState::Open;
	}
	else if (State == EWardrobeState::Closing)
	{
		State = EWardrobeState::Closed;
	}
	else
	{
		return;
	}
	// Re-enable the toggle now that the slide has settled.
	Prompt->SetActionAllowed(OpenActionId, true);
}

void ABTWardrobe::HandleDoorProgress(float Alpha)
{
	// Slide only the door panel aside (Prey-style sliding door); the cabinet body stays put.
	DoorMesh->SetRelativeLocation(ClosedLocation + SlideOffset * Alpha);
}
