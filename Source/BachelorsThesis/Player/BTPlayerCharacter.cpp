// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPlayerCharacter.h"

#include "BTInteractionDispatcherComponent.h"
#include "BTInteractionLog.h"
#include "BTPlayerInputComponent.h"
#include "BTPlayerUIComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

ABTPlayerCharacter::ABTPlayerCharacter()
{
	// Mirror the FPS-template character setup so the first-person arms read identically:
	// the FP mesh attaches to the body mesh and the camera rides its "head" socket.
	GetCapsuleComponent()->InitCapsuleSize(34.f, 96.f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	InteractionDispatcher = CreateDefaultSubobject<UBTInteractionDispatcherComponent>(TEXT("InteractionDispatcher"));
	PlayerInputService = CreateDefaultSubobject<UBTPlayerInputComponent>(TEXT("PlayerInputService"));
	PlayerUIService = CreateDefaultSubobject<UBTPlayerUIComponent>(TEXT("PlayerUIService"));
}

void ABTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add the movement/look mapping context for this character's own controls.
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ABTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABTPlayerCharacter::MoveInput);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABTPlayerCharacter::LookInput);
		}
		if (MouseLookAction)
		{
			EnhancedInput->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABTPlayerCharacter::LookInput);
		}
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
	}
	else
	{
		UE_LOG(LogInteractionSystem, Warning, TEXT("ABTPlayerCharacter expects an Enhanced Input component."));
	}
}

void ABTPlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void ABTPlayerCharacter::LookInput(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}
