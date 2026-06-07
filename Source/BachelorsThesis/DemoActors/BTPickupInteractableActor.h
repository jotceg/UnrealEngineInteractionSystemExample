// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTPickupInteractableActor.generated.h"

class UStaticMeshComponent;
class UBTInteractableComponent;
class UBTPickupBehaviorComponent;
class UBTHighlightComponent;

/**
 * DI-2 demonstration: a free-physics pickup. Composes the framework interactable component, the
 * pickup behaviour helper (grab/carry/drop/throw), a physics-simulating mesh, and the optional
 * passive highlight. The pickup helper takes over the mesh transform while carried and restores
 * physics on drop/throw, so the actor itself stays empty beyond the constructor. See ARCHITECTURE.md 7.2.
 */
UCLASS()
class BACHELORSTHESIS_API ABTPickupInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ABTPickupInteractableActor();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTPickupBehaviorComponent> PickupBehavior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBTHighlightComponent> Highlight;
};
