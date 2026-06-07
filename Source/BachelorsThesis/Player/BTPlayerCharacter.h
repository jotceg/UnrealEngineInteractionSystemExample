// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BTPlayerCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class UBTInteractionDispatcherComponent;
class UBTPlayerInputComponent;
class UBTPlayerUIComponent;
struct FInputActionValue;

/**
 * First-person player for the interaction demo. Derives directly from ACharacter (not from the
 * FPS-template character) and recreates only what the interaction system needs: a first-person
 * camera, an optional first-person mesh, and basic move/look/jump via Enhanced Input. It hosts the
 * three project-owned components: the dispatcher and the input/UI services. Asset slots
 * (mesh, mapping context, input actions) are wired on a Blueprint-derived class.
 */
UCLASS()
class BACHELORSTHESIS_API ABTPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABTPlayerCharacter();

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);

private:
	/** First-person camera; the dispatcher uses it as the raycast origin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	/** First-person arms mesh (mesh asset assigned on the Blueprint child). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	/** Player-side framework component: detection raycast and event dispatch. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTInteractionDispatcherComponent> InteractionDispatcher;

	/** Player service wrapping Enhanced Input for reaction helpers. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTPlayerInputComponent> PlayerInputService;

	/** Player service for cursor, prompt, and world-widget interaction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTPlayerUIComponent> PlayerUIService;

	/** Movement/look mapping context, added on possession. Assigned on the Blueprint child. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
};
