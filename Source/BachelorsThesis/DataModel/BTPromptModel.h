// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "BTPromptModel.generated.h"

/**
 * A single action option shown in a rich prompt (DI-3).
 * Built by UBTPromptUIComponent from its registered actions and rendered by the
 * generic prompt widget owned by UBTPlayerUIComponent.
 */
USTRUCT(BlueprintType)
struct FBTPromptOption
{
	GENERATED_BODY()

	/** Stable identifier of the action this option represents. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	FName ActionId;

	/** Human-readable label shown next to the key (localised text). */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	FText Label;

	/** Key the player presses to trigger this action. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	FKey DisplayKey;

	/** Whether the action is currently allowed. When false the option renders as disallowed. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	bool bIsAllowed = true;

	/** Reason shown when bIsAllowed is false (e.g. "Locked"). */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	FText DisallowedReason;
};

/**
 * Data passed from UBTPromptUIComponent to UBTPlayerUIComponent::ShowPrompt describing
 * what the prompt widget should display. Data-driven: one generic widget renders any model,
 * so a new interactable only configures its prompt component and provides callbacks (NFR-1, NFR-4).
 */
USTRUCT(BlueprintType)
struct FBTPromptModel
{
	GENERATED_BODY()

	/** Display name of the interactable object. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	FText ObjectName;

	/** Action options to render, in declaration order. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Prompt")
	TArray<FBTPromptOption> Options;
};
