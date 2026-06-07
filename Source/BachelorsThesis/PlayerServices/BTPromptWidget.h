// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BTPromptModel.h"
#include "BTPromptWidget.generated.h"

class UVerticalBox;
class UTextBlock;

/**
 * Generic rich-prompt widget for DI-3, built entirely in C++ (no UMG designer asset).
 * One instance renders any FBTPromptModel: object name plus a colour-coded line per action
 * option (allowed vs disallowed-with-reason). A new interactable type needs no new widget,
 * only its own UBTPromptUIComponent configuration (NFR-1, NFR-4). See ARCHITECTURE.md 5.4, 6.1.
 */
UCLASS()
class BACHELORSTHESIS_API UBTPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Rebuilds the displayed content from the given model. */
	void SetModel(const FBTPromptModel& Model);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	/** Colour for an allowed option line. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Prompt")
	FSlateColor AllowedColor = FSlateColor(FLinearColor::White);

	/** Colour for a disallowed option line (e.g. locked). */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Prompt")
	FSlateColor DisallowedColor = FSlateColor(FLinearColor::Red);

	/** Procedurally created object-name label. */
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ObjectNameText;

	/** Procedurally created container that holds one line per option. */
	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> OptionsBox;

	/** Builds a single option line "[Key] Label" (with reason when disallowed). */
	UTextBlock* BuildOptionLine(const FBTPromptOption& Option) const;
};
