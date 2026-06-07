// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "BTInteractionDispatcherComponent.h"
#include "BTInteractableComponent.h"
#include "BTTestSubscriberComponent.h"
#include "BTTestDoubles.h"
#include "BTTestCommon.h"
#include "BTActionCallback.h"
#include "BTPromptModel.h"
#include "BTCursorMode.h"
#include "BTPlayerInputComponent.h"
#include "InputAction.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_AUTOMATION_TESTS

namespace
{
	/** Builds a game world with a dispatcher and two interactable actors (A, B), each carrying a
	 *  labelled test subscriber. Clears the shared event log. Caller destroys the world. */
	void BuildDispatcherFixture(UWorld*& World, UBTInteractionDispatcherComponent*& Dispatcher,
		UBTInteractableComponent*& A, UBTInteractableComponent*& B)
	{
		World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
		AActor* Host = World->SpawnActor<AActor>();
		Dispatcher = NewObject<UBTInteractionDispatcherComponent>(Host);

		AActor* ActorA = World->SpawnActor<AActor>();
		A = NewObject<UBTInteractableComponent>(ActorA);
		A->RegisterComponent();
		UBTTestSubscriberComponent* SubA = NewObject<UBTTestSubscriberComponent>(ActorA);
		SubA->Label = TEXT("A");
		SubA->RegisterComponent();

		AActor* ActorB = World->SpawnActor<AActor>();
		B = NewObject<UBTInteractableComponent>(ActorB);
		B->RegisterComponent();
		UBTTestSubscriberComponent* SubB = NewObject<UBTTestSubscriberComponent>(ActorB);
		SubB->Label = TEXT("B");
		SubB->RegisterComponent();

		UBTTestSubscriberComponent::Events.Reset();
	}
}

// --- FR-1: per-type detection distance ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDetectionAcceptsHitWithinThresholdTest,
	"BachelorsThesis.Interaction.DetectionAcceptsHitWithinThreshold", BTTestFlags)
bool FBTDetectionAcceptsHitWithinThresholdTest::RunTest(const FString& Parameters)
{
	TestTrue(TEXT("Hit at the threshold is accepted"), UBTInteractionDispatcherComponent::IsWithinDetectionDistance(200, 200.f));
	TestTrue(TEXT("Closer hit is accepted"), UBTInteractionDispatcherComponent::IsWithinDetectionDistance(200, 50.f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDetectionRejectsHitBeyondThresholdTest,
	"BachelorsThesis.Interaction.DetectionRejectsHitBeyondThreshold", BTTestFlags)
bool FBTDetectionRejectsHitBeyondThresholdTest::RunTest(const FString& Parameters)
{
	TestFalse(TEXT("Hit just beyond the threshold is rejected"), UBTInteractionDispatcherComponent::IsWithinDetectionDistance(200, 200.1f));
	TestFalse(TEXT("Far hit is rejected"), UBTInteractionDispatcherComponent::IsWithinDetectionDistance(200, 1000.f));
	return true;
}

// --- FR-1/FR-3: dispatcher transition events ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDispatcherFiresDetectedOnFirstInteractableTest,
	"BachelorsThesis.Interaction.DispatcherFiresDetectedOnFirstInteractable", BTTestFlags)
bool FBTDispatcherFiresDetectedOnFirstInteractableTest::RunTest(const FString& Parameters)
{
	UWorld* World = nullptr; UBTInteractionDispatcherComponent* D = nullptr;
	UBTInteractableComponent* A = nullptr; UBTInteractableComponent* B = nullptr;
	BuildDispatcherFixture(World, D, A, B);

	D->TestSetCurrentInteractable(A);
	TestEqual(TEXT("One event fired"), UBTTestSubscriberComponent::Events.Num(), 1);
	if (UBTTestSubscriberComponent::Events.Num() == 1)
	{
		TestEqual(TEXT("It is A detected"), UBTTestSubscriberComponent::Events[0], FString(TEXT("A:Detected")));
	}

	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDispatcherIgnoresRedetectionOfSameInteractableTest,
	"BachelorsThesis.Interaction.DispatcherIgnoresRedetectionOfSameInteractable", BTTestFlags)
bool FBTDispatcherIgnoresRedetectionOfSameInteractableTest::RunTest(const FString& Parameters)
{
	UWorld* World = nullptr; UBTInteractionDispatcherComponent* D = nullptr;
	UBTInteractableComponent* A = nullptr; UBTInteractableComponent* B = nullptr;
	BuildDispatcherFixture(World, D, A, B);

	D->TestSetCurrentInteractable(A);
	UBTTestSubscriberComponent::Events.Reset();
	D->TestSetCurrentInteractable(A);
	TestEqual(TEXT("Re-detecting the same interactable fires nothing"), UBTTestSubscriberComponent::Events.Num(), 0);

	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDispatcherFiresLostBeforeDetectedOnDirectSwitchTest,
	"BachelorsThesis.Interaction.DispatcherFiresLostBeforeDetectedOnDirectSwitch", BTTestFlags)
bool FBTDispatcherFiresLostBeforeDetectedOnDirectSwitchTest::RunTest(const FString& Parameters)
{
	UWorld* World = nullptr; UBTInteractionDispatcherComponent* D = nullptr;
	UBTInteractableComponent* A = nullptr; UBTInteractableComponent* B = nullptr;
	BuildDispatcherFixture(World, D, A, B);

	D->TestSetCurrentInteractable(A);
	UBTTestSubscriberComponent::Events.Reset();
	D->TestSetCurrentInteractable(B);

	TestEqual(TEXT("Two events on A->B switch"), UBTTestSubscriberComponent::Events.Num(), 2);
	if (UBTTestSubscriberComponent::Events.Num() == 2)
	{
		TestEqual(TEXT("A is lost first"), UBTTestSubscriberComponent::Events[0], FString(TEXT("A:Lost")));
		TestEqual(TEXT("B is detected second"), UBTTestSubscriberComponent::Events[1], FString(TEXT("B:Detected")));
	}

	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDispatcherFiresLostWhenInteractableClearedTest,
	"BachelorsThesis.Interaction.DispatcherFiresLostWhenInteractableCleared", BTTestFlags)
bool FBTDispatcherFiresLostWhenInteractableClearedTest::RunTest(const FString& Parameters)
{
	UWorld* World = nullptr; UBTInteractionDispatcherComponent* D = nullptr;
	UBTInteractableComponent* A = nullptr; UBTInteractableComponent* B = nullptr;
	BuildDispatcherFixture(World, D, A, B);

	D->TestSetCurrentInteractable(A);
	UBTTestSubscriberComponent::Events.Reset();
	D->TestSetCurrentInteractable(nullptr);

	TestEqual(TEXT("One event when cleared"), UBTTestSubscriberComponent::Events.Num(), 1);
	if (UBTTestSubscriberComponent::Events.Num() == 1)
	{
		TestEqual(TEXT("It is A lost"), UBTTestSubscriberComponent::Events[0], FString(TEXT("A:Lost")));
	}

	World->DestroyWorld(false);
	return true;
}

// --- DI-3: rich prompt model + object-side validation ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTPromptModelReflectsValidationTest,
	"BachelorsThesis.Interaction.PromptModelReflectsValidation", BTTestFlags)
bool FBTPromptModelReflectsValidationTest::RunTest(const FString& Parameters)
{
	// Detecting binds the registered actions; without a real Enhanced Input system (headless test)
	// the input service logs an expected, harmless warning per action. Whitelist it so the test is green.
	AddExpectedError(TEXT("no Enhanced Input component available"), EAutomationExpectedErrorFlags::Contains, 0);

	UBTTestPromptComponent* Prompt = NewObject<UBTTestPromptComponent>(GetTransientPackage());
	UInputAction* OpenAction = NewObject<UInputAction>(GetTransientPackage());
	UInputAction* LockAction = NewObject<UInputAction>(GetTransientPackage());

	FBTActionCallback Noop;
	Prompt->RegisterAction(FName("Open"), FText::FromString(TEXT("Open")), OpenAction, Noop, true);
	Prompt->RegisterAction(FName("Lock"), FText::FromString(TEXT("Lock")), LockAction, Noop, true);
	Prompt->SetActionAllowed(FName("Lock"), false, FText::FromString(TEXT("Locked")));

	UBTTestUIComponent* UI = NewObject<UBTTestUIComponent>(GetTransientPackage());
	UBTPlayerInputComponent* Input = NewObject<UBTPlayerInputComponent>(GetTransientPackage());
	UBTMockInteractor* Mock = NewObject<UBTMockInteractor>(GetTransientPackage());
	Mock->MockUI = UI;
	Mock->MockInput = Input;

	Prompt->TestDetect(Mock);

	TestTrue(TEXT("Prompt was shown"), UI->bPromptShown);
	TestEqual(TEXT("Model has both options"), UI->CapturedModel.Options.Num(), 2);

	const FBTPromptOption* Open = UI->CapturedModel.Options.FindByPredicate(
		[](const FBTPromptOption& O) { return O.ActionId == FName("Open"); });
	const FBTPromptOption* Lock = UI->CapturedModel.Options.FindByPredicate(
		[](const FBTPromptOption& O) { return O.ActionId == FName("Lock"); });

	if (TestNotNull(TEXT("Open option present"), Open))
	{
		TestTrue(TEXT("Open is allowed"), Open->bIsAllowed);
	}
	if (TestNotNull(TEXT("Lock option present"), Lock))
	{
		TestFalse(TEXT("Lock is disallowed"), Lock->bIsAllowed);
		TestEqual(TEXT("Lock shows its reason"), Lock->DisallowedReason.ToString(), FString(TEXT("Locked")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTDisallowedActionDoesNotInvokeCallbackTest,
	"BachelorsThesis.Interaction.DisallowedActionDoesNotInvokeCallback", BTTestFlags)
bool FBTDisallowedActionDoesNotInvokeCallbackTest::RunTest(const FString& Parameters)
{
	UBTTestPromptComponent* Prompt = NewObject<UBTTestPromptComponent>(GetTransientPackage());
	UInputAction* OpenAction = NewObject<UInputAction>(GetTransientPackage());

	bool bCalled = false;
	FBTActionCallback Callback;
	Callback.BindLambda([&bCalled]() { bCalled = true; });
	Prompt->RegisterAction(FName("Open"), FText::FromString(TEXT("Open")), OpenAction, Callback, true);

	Prompt->SetActionAllowed(FName("Open"), false);
	Prompt->TestTrigger(OpenAction);
	TestFalse(TEXT("Disallowed action does not invoke the callback"), bCalled);

	Prompt->SetActionAllowed(FName("Open"), true);
	Prompt->TestTrigger(OpenAction);
	TestTrue(TEXT("Allowed action invokes the callback"), bCalled);

	return true;
}

// --- DI-1: screen surface cursor + widget interaction ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTScreenEnablesWidgetAndSetsCursorOnDetectTest,
	"BachelorsThesis.Interaction.ScreenEnablesWidgetAndSetsCursorOnDetect", BTTestFlags)
bool FBTScreenEnablesWidgetAndSetsCursorOnDetectTest::RunTest(const FString& Parameters)
{
	UBTTestScreenComponent* Screen = NewObject<UBTTestScreenComponent>(GetTransientPackage());
	UBTTestUIComponent* UI = NewObject<UBTTestUIComponent>(GetTransientPackage());
	UBTPlayerInputComponent* Input = NewObject<UBTPlayerInputComponent>(GetTransientPackage());
	UBTMockInteractor* Mock = NewObject<UBTMockInteractor>(GetTransientPackage());
	Mock->MockUI = UI;
	Mock->MockInput = Input;

	Screen->TestDetect(Mock);

	TestEqual(TEXT("Cursor switches to OnWidget"), static_cast<int32>(UI->LastCursorMode), static_cast<int32>(EBTCursorMode::OnWidget));
	TestTrue(TEXT("Widget interaction enabled"), UI->bWidgetEnabled);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTScreenDisablesAndResetsOnLostTest,
	"BachelorsThesis.Interaction.ScreenDisablesAndResetsOnLost", BTTestFlags)
bool FBTScreenDisablesAndResetsOnLostTest::RunTest(const FString& Parameters)
{
	UBTTestScreenComponent* Screen = NewObject<UBTTestScreenComponent>(GetTransientPackage());
	UBTTestUIComponent* UI = NewObject<UBTTestUIComponent>(GetTransientPackage());
	UBTPlayerInputComponent* Input = NewObject<UBTPlayerInputComponent>(GetTransientPackage());
	UBTMockInteractor* Mock = NewObject<UBTMockInteractor>(GetTransientPackage());
	Mock->MockUI = UI;
	Mock->MockInput = Input;

	Screen->TestDetect(Mock);
	Screen->TestLost(Mock);

	TestFalse(TEXT("Widget interaction disabled on loss"), UI->bWidgetEnabled);
	TestEqual(TEXT("Cursor reset to Default"), static_cast<int32>(UI->LastCursorMode), static_cast<int32>(EBTCursorMode::Default));
	return true;
}

// --- DI-2: pickup cursor feedback on detect/lose ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTPickupSetsGrabHandCursorOnDetectTest,
	"BachelorsThesis.Interaction.PickupSetsGrabHandCursorOnDetect", BTTestFlags)
bool FBTPickupSetsGrabHandCursorOnDetectTest::RunTest(const FString& Parameters)
{
	UBTTestPickupComponent* Pickup = NewObject<UBTTestPickupComponent>(GetTransientPackage());
	UBTTestUIComponent* UI = NewObject<UBTTestUIComponent>(GetTransientPackage());
	UBTPlayerInputComponent* Input = NewObject<UBTPlayerInputComponent>(GetTransientPackage());
	UBTMockInteractor* Mock = NewObject<UBTMockInteractor>(GetTransientPackage());
	Mock->MockUI = UI;
	Mock->MockInput = Input;

	Pickup->TestDetect(Mock);

	TestEqual(TEXT("Cursor switches to GrabHand"), static_cast<int32>(UI->LastCursorMode), static_cast<int32>(EBTCursorMode::GrabHand));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTPickupResetsCursorOnLostTest,
	"BachelorsThesis.Interaction.PickupResetsCursorOnLost", BTTestFlags)
bool FBTPickupResetsCursorOnLostTest::RunTest(const FString& Parameters)
{
	UBTTestPickupComponent* Pickup = NewObject<UBTTestPickupComponent>(GetTransientPackage());
	UBTTestUIComponent* UI = NewObject<UBTTestUIComponent>(GetTransientPackage());
	UBTPlayerInputComponent* Input = NewObject<UBTPlayerInputComponent>(GetTransientPackage());
	UBTMockInteractor* Mock = NewObject<UBTMockInteractor>(GetTransientPackage());
	Mock->MockUI = UI;
	Mock->MockInput = Input;

	Pickup->TestDetect(Mock);
	Pickup->TestLost(Mock);

	TestEqual(TEXT("Cursor reset to Default"), static_cast<int32>(UI->LastCursorMode), static_cast<int32>(EBTCursorMode::Default));
	return true;
}

#endif // WITH_AUTOMATION_TESTS
