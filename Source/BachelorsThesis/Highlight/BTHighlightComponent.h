// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BTHighlightComponent.generated.h"

class UMaterialInterface;

/**
 * Realises FR-2 (passive interactivity signalling). Any actor that adds this component has a project
 * highlight material applied as an Overlay Material on its mesh primitives, rendered as an extra pass
 * over the mesh whenever it is in view.
 *
 * Opt-in per type and independent of the detection raycast. The component asserts in BeginPlay that
 * the owner is actually interactable (highlight without interaction is meaningless) and clears the
 * overlay on failure so the misconfiguration is visible. See ARCHITECTURE.md 3.4.
 *
 * Asset dependency: a project highlight material (M_BT_Highlight - unlit, translucent/additive,
 * pulsing) assigned to HighlightMaterial. Recorded in CREDITS.md. (Phase 2: the rendering mechanism
 * changed from a Custom Depth-stencil post-process to a per-mesh Overlay Material - see
 * DISCOVERED_GAPS.md Gap #10. The component's role as the opt-in FR-2 hook is unchanged.)
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BACHELORSTHESIS_API UBTHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBTHighlightComponent();

	/** Highlight material applied as an Overlay Material on the owner's mesh primitives. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Highlight")
	TObjectPtr<UMaterialInterface> HighlightMaterial;

protected:
	/** Applies the overlay material on the owner's mesh primitives (visible in editor too). */
	virtual void OnRegister() override;

	/** Asserts the owner has an IBTInteractable component; clears the overlay if not. */
	virtual void BeginPlay() override;

private:
	void SetOwnerHighlight(bool bEnabled);
};
