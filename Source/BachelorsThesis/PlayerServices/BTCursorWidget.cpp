// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCursorWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

TSharedRef<SWidget> UBTCursorWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CursorRoot"));
		WidgetTree->RootWidget = RootCanvas;

		ReticleImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Reticle"));

		UCanvasPanelSlot* ReticleSlot = RootCanvas->AddChildToCanvas(ReticleImage);
		ReticleSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		ReticleSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ReticleSlot->SetAutoSize(true);

		ApplyCursorMode(EBTCursorMode::Default);
	}

	return Super::RebuildWidget();
}

void UBTCursorWidget::ApplyCursorMode(EBTCursorMode Mode)
{
	if (!ReticleImage)
	{
		return;
	}

	const TObjectPtr<UTexture2D>* Texture = CursorTextures.Find(Mode);
	if (Texture && *Texture)
	{
		ReticleImage->SetBrushFromTexture(*Texture, /*bMatchSize*/ false);
		ReticleImage->SetDesiredSizeOverride(CursorSize);
		ReticleImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		// No texture configured for this mode: hide the reticle rather than draw a default brush.
		ReticleImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
