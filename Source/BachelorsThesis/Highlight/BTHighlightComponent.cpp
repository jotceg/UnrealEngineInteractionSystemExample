// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTHighlightComponent.h"

#include "BTInteractable.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"

UBTHighlightComponent::UBTHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBTHighlightComponent::OnRegister()
{
	Super::OnRegister();
	SetOwnerHighlight(true);
}

void UBTHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	const bool bHasInteractable = Owner && Owner->FindComponentByInterface(UBTInteractable::StaticClass()) != nullptr;

	if (!ensureMsgf(bHasInteractable,
		TEXT("UBTHighlightComponent on %s has no IBTInteractable component; highlight without interaction is invalid."),
		Owner ? *Owner->GetName() : TEXT("<null>")))
	{
		// Clear the overlay so the misconfiguration is visible (no glow) rather than silent.
		SetOwnerHighlight(false);
	}
}

void UBTHighlightComponent::SetOwnerHighlight(bool bEnabled)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UMaterialInterface* const Material = bEnabled ? ToRawPtr(HighlightMaterial) : nullptr;
	Owner->ForEachComponent<UMeshComponent>(/*bIncludeFromChildActors*/ false,
		[Material](UMeshComponent* Mesh)
		{
			Mesh->SetOverlayMaterial(Material);
		});
}
