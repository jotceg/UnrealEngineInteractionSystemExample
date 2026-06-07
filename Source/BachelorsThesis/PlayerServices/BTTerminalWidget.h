// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BTTerminalWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UWidget;
class UTexture2D;

/**
 * Generic terminal screen widget for DI-1, built entirely in C++ (no UMG designer asset).
 * Two navigable views - a dashboard with three tiles (Messages / Files / System) and a detail
 * view with a back button - to demonstrate a real, clickable in-world UI. Clicks come from the
 * player's UWidgetInteractionComponent (mouse-style projection) or the F key (see
 * UBTScreenSurfaceComponent). Content is generic placeholder text; no third-party trademarks.
 */
UCLASS()
class BACHELORSTHESIS_API UBTTerminalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Icons")
	TObjectPtr<UTexture2D> IconMessages;

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Icons")
	TObjectPtr<UTexture2D> IconFiles;

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Icons")
	TObjectPtr<UTexture2D> IconSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Icons")
	TObjectPtr<UTexture2D> IconBack;

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Style")
	FLinearColor BackgroundColor = FLinearColor(0.93f, 0.95f, 0.97f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Style")
	FLinearColor AccentColor = FLinearColor(0.0f, 0.62f, 0.66f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Style")
	FLinearColor TextColor = FLinearColor(0.10f, 0.12f, 0.15f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Header")
	FText UserName = FText::FromString(TEXT("UZYTKOWNIK: A. KOWALSKI"));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Header")
	FText ClockLabel = FText::FromString(TEXT("07:32"));

	// Displayed strings are English placeholders by design; set the final Polish text (with
	// diacritics) in the editor, where Unicode is stored safely without source-encoding risk.
	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText MessagesLabel = FText::FromString(TEXT("MESSAGES"));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText FilesLabel = FText::FromString(TEXT("FILES"));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText SystemLabel = FText::FromString(TEXT("SYSTEM"));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText MessagesBody = FText::FromString(TEXT("Welcome aboard. Your onboarding tasks are ready.\n\nFrom: J. Nowak\nSubject: First day"));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText FilesBody = FText::FromString(TEXT("No files to display."));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText SystemBody = FText::FromString(TEXT("Status: nominal.\nUpdates: 2 pending.\nDiagnostics: OK."));

	UPROPERTY(EditDefaultsOnly, Category = "Terminal|Content")
	FText BackLabel = FText::FromString(TEXT("Back"));

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION() void ShowMessages();
	UFUNCTION() void ShowFiles();
	UFUNCTION() void ShowSystem();
	UFUNCTION() void ShowDashboard();

private:
	UPROPERTY(Transient) TObjectPtr<UWidget> DashboardPanel;
	UPROPERTY(Transient) TObjectPtr<UWidget> DetailPanel;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> DetailTitle;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> DetailBody;
	UPROPERTY(Transient) TObjectPtr<UImage> DetailIcon;

	void ShowDetail(const FText& Title, const FText& Body, UTexture2D* Icon);

	UWidget* BuildDashboard();
	UWidget* BuildDetail();
	UButton* BuildTile(UTexture2D* Icon, const FText& Count, const FText& Label);
	UTextBlock* MakeText(const FText& Text, int32 Size, const FLinearColor& Color, bool bBold = false);
	UImage* MakeColorImage(const FLinearColor& Color, const FVector2D& Size);
	UImage* MakeIcon(UTexture2D* Texture, float Size);
	void StyleTileButton(UButton* Button);
};
