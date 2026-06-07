// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTScreenInteractableActor.h"

#include "BTInteractableComponent.h"
#include "BTScreenSurfaceComponent.h"
#include "BTTerminalWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

ABTScreenInteractableActor::ABTScreenInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	RootComponent = ScreenMesh;

	ScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ScreenWidget"));
	ScreenWidget->SetupAttachment(ScreenMesh);
	ScreenWidget->SetWidgetSpace(EWidgetSpace::World);
	// Default to the C++ terminal UI at a 16:9 draw size; a Blueprint child can override either.
	ScreenWidget->SetWidgetClass(UBTTerminalWidget::StaticClass());
	ScreenWidget->SetDrawSize(FVector2D(1024.f, 576.f));

	Interactable = CreateDefaultSubobject<UBTInteractableComponent>(TEXT("Interactable"));
	ScreenSurface = CreateDefaultSubobject<UBTScreenSurfaceComponent>(TEXT("ScreenSurface"));
}
