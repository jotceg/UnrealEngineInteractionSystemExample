// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "BTBindingHandle.h"
#include "BTCursorMode.h"
#include "BTInteractable.h"
#include "BTInteractableComponent.h"
#include "BTPlayerUIComponent.h"
#include "BTTestSubscriberComponent.h"
#include "BTTestCommon.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_AUTOMATION_TESTS

/**
 * FR-3 / NFR-1: UBTInteractableComponent must broadcast detection events to every
 * IBTInteractableSubscriber on its owning actor, with no hard reference in either direction.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTInteractableDispatchesToSubscribersTest,
	"BachelorsThesis.Interaction.InteractableDispatchesToSubscribers", BTTestFlags)

bool FBTInteractableDispatchesToSubscribersTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
	if (!TestNotNull(TEXT("Test world created"), World))
	{
		return false;
	}

	AActor* HostActor = World->SpawnActor<AActor>();
	TestNotNull(TEXT("Host actor spawned"), HostActor);

	UBTInteractableComponent* Interactable = NewObject<UBTInteractableComponent>(HostActor);
	Interactable->RegisterComponent();

	UBTTestSubscriberComponent* Subscriber = NewObject<UBTTestSubscriberComponent>(HostActor);
	Subscriber->RegisterComponent();

	IBTInteractable* AsInteractable = Cast<IBTInteractable>(Interactable);
	if (TestNotNull(TEXT("Component implements IBTInteractable"), AsInteractable))
	{
		AsInteractable->OnDetected(nullptr);
		TestEqual(TEXT("Subscriber received one detected event"), Subscriber->DetectedCount, 1);
		TestEqual(TEXT("No loss event yet"), Subscriber->LostCount, 0);

		AsInteractable->OnLost(nullptr);
		TestEqual(TEXT("Subscriber received one loss event"), Subscriber->LostCount, 1);
		TestEqual(TEXT("Still one detected event"), Subscriber->DetectedCount, 1);
	}

	World->DestroyWorld(/*bInformEngineOfWorld*/ false);
	return true;
}

/**
 * Cursor-mode switching at the service level: SetCursorMode stores the mode and GetCursorMode
 * returns it (the visual swap is exercised in PIE).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTCursorModeRoundTripTest,
	"BachelorsThesis.Interaction.CursorModeRoundTrip", BTTestFlags)

bool FBTCursorModeRoundTripTest::RunTest(const FString& Parameters)
{
	UBTPlayerUIComponent* UIService = NewObject<UBTPlayerUIComponent>(GetTransientPackage());
	if (!TestNotNull(TEXT("UI service created"), UIService))
	{
		return false;
	}

	TestEqual(TEXT("Defaults to Default cursor"), static_cast<int32>(UIService->GetCursorMode()), static_cast<int32>(EBTCursorMode::Default));

	UIService->SetCursorMode(EBTCursorMode::GrabHand);
	TestEqual(TEXT("Switches to GrabHand"), static_cast<int32>(UIService->GetCursorMode()), static_cast<int32>(EBTCursorMode::GrabHand));

	UIService->SetCursorMode(EBTCursorMode::OnWidget);
	TestEqual(TEXT("Switches to OnWidget"), static_cast<int32>(UIService->GetCursorMode()), static_cast<int32>(EBTCursorMode::OnWidget));

	UIService->SetCursorMode(EBTCursorMode::Default);
	TestEqual(TEXT("Resets to Default"), static_cast<int32>(UIService->GetCursorMode()), static_cast<int32>(EBTCursorMode::Default));

	return true;
}

/**
 * FBTBindingHandle wraps an engine handle: validity and equality must behave as expected so the
 * input-aware base can track and release bindings.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTBindingHandleSemanticsTest,
	"BachelorsThesis.Interaction.BindingHandleSemantics", BTTestFlags)

bool FBTBindingHandleSemanticsTest::RunTest(const FString& Parameters)
{
	const FBTBindingHandle DefaultHandle;
	TestFalse(TEXT("Default handle is invalid"), DefaultHandle.IsValid());

	const FBTBindingHandle HandleA(7);
	const FBTBindingHandle HandleB(7);
	const FBTBindingHandle HandleC(8);

	TestTrue(TEXT("Explicit handle is valid"), HandleA.IsValid());
	TestTrue(TEXT("Equal ids compare equal"), HandleA == HandleB);
	TestFalse(TEXT("Different ids compare unequal"), HandleA == HandleC);

	return true;
}

#endif // WITH_AUTOMATION_TESTS
