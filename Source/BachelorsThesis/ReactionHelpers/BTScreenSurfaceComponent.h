// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTInputAwareInteractionComponent.h"
#include "BTScreenSurfaceComponent.generated.h"

class IBTInteractor;
class UInputAction;
class UBTPlayerUIComponent;
struct FInputActionValue;

/**
 * Concrete reaction helper for the screen variant (DI-1). While the host screen actor is detected,
 * it enables the player's world-widget interaction so the cursor projects onto the in-world widget
 * and sets the cursor mode to OnWidget. In Prey the screen is mouse-driven but also clickable with
 * the interact key, so this helper inherits the input-aware base and binds InteractAction (F) to a
 * simulated click on the widget under the crosshair. See ARCHITECTURE.md 4.5.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTScreenSurfaceComponent : public UBTInputAwareInteractionComponent
{
	GENERATED_BODY()

public:
	/** Key (F) that clicks the widget element under the crosshair, alongside mouse-style projection. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Screen")
	TObjectPtr<UInputAction> InteractAction;

protected:
	virtual void OnInteractableDetected(IBTInteractor* Interactor) override;
	virtual void OnInteractableLost(IBTInteractor* Interactor) override;

	UFUNCTION()
	void OnInteractTriggered(FInputActionValue Value, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction);

private:
	/** UI service of the player aiming at this screen; valid only between detect and loss. */
	TWeakObjectPtr<UBTPlayerUIComponent> ActiveUIService;
};
