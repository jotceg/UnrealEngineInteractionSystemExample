// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTPromptWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#define LOCTEXT_NAMESPACE "BTPrompt"

TSharedRef<SWidget> UBTPromptWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("PromptRoot"));
		WidgetTree->RootWidget = RootCanvas;

		UVerticalBox* Container = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PromptContainer"));
		UCanvasPanelSlot* ContainerSlot = RootCanvas->AddChildToCanvas(Container);
		// Anchor slightly below screen centre, matching the reference game's prompt placement.
		ContainerSlot->SetAnchors(FAnchors(0.5f, 0.6f));
		ContainerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ContainerSlot->SetAutoSize(true);

		ObjectNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ObjectName"));
		Container->AddChildToVerticalBox(ObjectNameText);

		OptionsBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Options"));
		Container->AddChildToVerticalBox(OptionsBox);
	}

	return Super::RebuildWidget();
}

void UBTPromptWidget::SetModel(const FBTPromptModel& Model)
{
	if (!ObjectNameText || !OptionsBox)
	{
		return;
	}

	ObjectNameText->SetText(Model.ObjectName);

	OptionsBox->ClearChildren();
	for (const FBTPromptOption& Option : Model.Options)
	{
		if (UTextBlock* Line = BuildOptionLine(Option))
		{
			OptionsBox->AddChildToVerticalBox(Line);
		}
	}
}

UTextBlock* UBTPromptWidget::BuildOptionLine(const FBTPromptOption& Option) const
{
	UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	if (!Line)
	{
		return nullptr;
	}

	const FText KeyName = Option.DisplayKey.GetDisplayName();
	FText LineText;
	if (Option.bIsAllowed)
	{
		LineText = FText::Format(LOCTEXT("OptionAllowed", "[{0}] {1}"), KeyName, Option.Label);
	}
	else
	{
		LineText = FText::Format(LOCTEXT("OptionDisallowed", "[{0}] {1} ({2})"), KeyName, Option.Label, Option.DisallowedReason);
	}

	Line->SetText(LineText);
	Line->SetColorAndOpacity(Option.bIsAllowed ? AllowedColor : DisallowedColor);
	return Line;
}

#undef LOCTEXT_NAMESPACE
