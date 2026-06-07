// Copyright Epic Games, Inc. All Rights Reserved.


#include "BachelorsThesisPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "BachelorsThesisCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "BachelorsThesis.h"
#include "Widgets/Input/SVirtualJoystick.h"

ABachelorsThesisPlayerController::ABachelorsThesisPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ABachelorsThesisCameraManager::StaticClass();
}

void ABachelorsThesisPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogBachelorsThesis, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ABachelorsThesisPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool ABachelorsThesisPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
