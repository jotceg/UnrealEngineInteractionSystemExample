// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTWardrobe.generated.h"

class UStaticMeshComponent;
class UTimelineComponent;
class UCurveFloat;
class UInputAction;
class UBTInteractableComponent;
class UBTPromptUIComponent;

/**
 * DI-3 demonstration: a rich-prompt interactable (locked-door variant). Composes the framework
 * interactable component, the prompt reaction helper, body and door meshes, and a timeline for the
 * door slide. Registers one action (Open) in BeginPlay; the bIsLocked flag drives object-side
 * validation via SetActionAllowed (red prompt, key does nothing while locked). See ARCHITECTURE.md 7.3.
 */
UCLASS()
class BACHELORSTHESIS_API ABTWardrobe : public AActor
{
	GENERATED_BODY()

public:
	ABTWardrobe();

protected:
	virtual void BeginPlay() override;

	/** Callback invoked by the prompt helper when the player triggers Open and it is allowed. */
	void OnOpenActionExecuted();

	/** Timeline progress handler: slides the wardrobe from closed to SlideOffset by Alpha. */
	UFUNCTION()
	void HandleDoorProgress(float Alpha);

	/** Timeline finished handler: settles the state to Open/Closed and re-enables the toggle. */
	UFUNCTION()
	void OnSlideFinished();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> DoorTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTPromptUIComponent> Prompt;

	/** Normalised 0..1 curve driving the door slide. Assigned on the Blueprint child. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UCurveFloat> DoorCurve;

	/** Input action that triggers Open. Assigned on the Blueprint child. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UInputAction> OpenAction;

	/** When true, Open is disallowed and the prompt shows LockedReason. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsLocked = false;

	/** Local offset the wardrobe slides to when fully open (Prey-style pull-aside, not a swing). */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FVector SlideOffset = FVector(0.f, -110.f, 0.f);

	/** Display name shown in the prompt header. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText DisplayName;

	/** Label of the Open action in the prompt. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText OpenLabel;

	/** Reason shown when locked. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText LockedReason;

	/** Prompt info shown once the wardrobe has been opened (disables re-opening, Prey-style). */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText OpenedReason;

	/** Wardrobe location captured at BeginPlay, used as the closed baseline for the slide. */
	FVector ClosedLocation;

	/** Open/close toggle state; transitions are locked while the slide animates. */
	enum class EWardrobeState : uint8 { Closed, Opening, Open, Closing };
	EWardrobeState State = EWardrobeState::Closed;
};
