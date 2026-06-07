// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTScreenInteractableActor.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UBTInteractableComponent;
class UBTScreenSurfaceComponent;

/**
 * DI-1 demonstration: a screen-style interactable. Composes the framework interactable component,
 * the screen-surface reaction helper, a mesh, and an in-world widget. All cursor projection and
 * click simulation happen via the helper plus the player's UWidgetInteractionComponent, so the
 * actor itself is just the visual host for the widget. See ARCHITECTURE.md 7.1.
 */
UCLASS()
class BACHELORSTHESIS_API ABTScreenInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ABTScreenInteractableActor();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ScreenMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> ScreenWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTScreenSurfaceComponent> ScreenSurface;
};
