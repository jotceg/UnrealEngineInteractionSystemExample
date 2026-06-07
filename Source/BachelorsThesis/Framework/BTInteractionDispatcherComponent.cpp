// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTInteractionDispatcherComponent.h"

#include "BTCursorMode.h"
#include "BTInteractable.h"
#include "BTInteractionLog.h"
#include "BTPerf.h"
#include "BTPlayerInputComponent.h"
#include "BTPlayerUIComponent.h"
#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UBTInteractionDispatcherComponent::UBTInteractionDispatcherComponent()
{
	// Detection is driven by the asynchronous trace callback, not by tick.
	PrimaryComponentTick.bCanEverTick = false;
}

void UBTInteractionDispatcherComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* Owner = GetOwner())
	{
		CachedCamera = Owner->FindComponentByClass<UCameraComponent>();
		CachedInputService = Owner->FindComponentByClass<UBTPlayerInputComponent>();
		CachedUIService = Owner->FindComponentByClass<UBTPlayerUIComponent>();
	}

	const UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}

	if (!CachedCamera)
	{
		UE_LOG(LogInteractionSystem, Warning, TEXT("Dispatcher found no camera on %s; detection disabled."),
			GetOwner() ? *GetOwner()->GetName() : TEXT("<null>"));
		return;
	}

	TraceDelegate.BindUObject(this, &UBTInteractionDispatcherComponent::OnTraceCompleted);
	RequestTrace();
}

void UBTInteractionDispatcherComponent::RequestTrace()
{
	UWorld* World = GetWorld();
	UCameraComponent* Camera = GetInteractorCamera();
	if (!World || !Camera)
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * MaxTraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BTInteractionTrace), /*bTraceComplex*/ false, GetOwner());
	TraceHandle = World->AsyncLineTraceByChannel(EAsyncTraceType::Single, Start, End,
		InteractableTraceChannel, Params, FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);
}

void UBTInteractionDispatcherComponent::OnTraceCompleted(const FTraceHandle& Handle, FTraceDatum& Datum)
{
	BT_PERF_SCOPE(STAT_BTTraceCallback, TraceCallback);

	// If the previously selected interactable was destroyed mid-interaction, its reaction helpers
	// (which set cursor/prompt) are gone too, so reset the player UI directly to avoid stuck state.
	if (CurrentInteractable.IsStale())
	{
		if (UBTPlayerUIComponent* UI = GetPlayerUIComponent())
		{
			UI->SetCursorMode(EBTCursorMode::Default);
			UI->HidePrompt();
		}
		CurrentInteractable = nullptr;
	}

	UActorComponent* NewInteractable = nullptr;
	for (const FHitResult& Hit : Datum.OutHits)
	{
		if (!Hit.bBlockingHit)
		{
			continue;
		}

		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			break;
		}

		UActorComponent* InteractableComponent = HitActor->FindComponentByInterface(UBTInteractable::StaticClass());
		if (const IBTInteractable* Interactable = Cast<IBTInteractable>(InteractableComponent))
		{
			// Reject hits beyond the object type's configured detection distance (FR-1).
			if (IsWithinDetectionDistance(Interactable->GetMaxDetectionDistance(), Hit.Distance))
			{
				NewInteractable = InteractableComponent;
			}
		}
		break; // only the first blocking hit matters
	}

	SetCurrentInteractable(NewInteractable);

	// Callback-recursive re-arm: immediately request the next trace (one trace per physics cycle).
	RequestTrace();
}

void UBTInteractionDispatcherComponent::SetCurrentInteractable(UActorComponent* NewInteractable)
{
	UActorComponent* PreviousInteractable = CurrentInteractable.Get();
	if (PreviousInteractable == NewInteractable)
	{
		return;
	}

	if (IBTInteractable* Previous = Cast<IBTInteractable>(PreviousInteractable))
	{
		Previous->OnLost(this);
	}

	CurrentInteractable = NewInteractable;

	if (IBTInteractable* Next = Cast<IBTInteractable>(NewInteractable))
	{
		Next->OnDetected(this);
	}
}

bool UBTInteractionDispatcherComponent::IsWithinDetectionDistance(int32 MaxDistance, float HitDistance)
{
	return HitDistance <= static_cast<float>(MaxDistance);
}

UCameraComponent* UBTInteractionDispatcherComponent::GetInteractorCamera() const
{
	if (CachedCamera)
	{
		return CachedCamera;
	}
	return GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
}

UBTPlayerInputComponent* UBTInteractionDispatcherComponent::GetPlayerInputComponent() const
{
	if (CachedInputService)
	{
		return CachedInputService;
	}
	return GetOwner() ? GetOwner()->FindComponentByClass<UBTPlayerInputComponent>() : nullptr;
}

UBTPlayerUIComponent* UBTInteractionDispatcherComponent::GetPlayerUIComponent() const
{
	if (CachedUIService)
	{
		return CachedUIService;
	}
	return GetOwner() ? GetOwner()->FindComponentByClass<UBTPlayerUIComponent>() : nullptr;
}

ACharacter* UBTInteractionDispatcherComponent::GetInteractorCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}
