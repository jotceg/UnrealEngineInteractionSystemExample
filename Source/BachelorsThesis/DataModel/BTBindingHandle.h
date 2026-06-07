// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTBindingHandle.generated.h"

/**
 * Opaque token returned by UBTPlayerInputComponent::BindAction and accepted by UnbindAction.
 * Wraps the engine's Enhanced Input binding handle (an int32) in a typed struct so reaction
 * helpers can track their active bindings without depending on engine input internals.
 */
USTRUCT()
struct FBTBindingHandle
{
	GENERATED_BODY()

	UPROPERTY()
	int32 InternalId = INDEX_NONE;

	FBTBindingHandle() = default;
	explicit FBTBindingHandle(int32 InId) : InternalId(InId) {}

	bool IsValid() const { return InternalId != INDEX_NONE; }

	bool operator==(const FBTBindingHandle& Other) const { return InternalId == Other.InternalId; }
};
