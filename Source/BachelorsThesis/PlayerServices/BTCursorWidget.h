// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BTCursorMode.h"
#include "BTCursorWidget.generated.h"

class UImage;
class UTexture2D;

/**
 * Screen-centred reticle whose image swaps with the current cursor mode (DI-1/DI-2 feedback).
 * Built entirely in C++ (no UMG designer asset). The author supplies one texture per mode as
 * references on a Blueprint-derived class; the widget tree itself is procedural. See ARCHITECTURE.md 6.3.
 */
UCLASS()
class BACHELORSTHESIS_API UBTCursorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Swaps the reticle image to the texture mapped for the given mode (or hides it if unmapped). */
	void ApplyCursorMode(EBTCursorMode Mode);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	/** One reticle texture per cursor mode, assigned on a Blueprint-derived class. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Cursor")
	TMap<EBTCursorMode, TObjectPtr<UTexture2D>> CursorTextures;

	/** On-screen size of the reticle in slate units. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Cursor")
	FVector2D CursorSize = FVector2D(32.f, 32.f);

	/** Procedurally created reticle image, refreshed by ApplyCursorMode. */
	UPROPERTY(Transient)
	TObjectPtr<UImage> ReticleImage;
};
