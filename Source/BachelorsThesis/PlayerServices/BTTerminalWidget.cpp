// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTTerminalWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/SlateTypes.h"

namespace
{
	FSlateChildSize FillSize()
	{
		FSlateChildSize Size;
		Size.SizeRule = ESlateSizeRule::Fill;
		return Size;
	}
}

TSharedRef<SWidget> UBTTerminalWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TerminalRoot"));
		WidgetTree->RootWidget = Root;

		// Full-bleed light background.
		UImage* Background = MakeColorImage(BackgroundColor, FVector2D(64.f, 64.f));
		if (UCanvasPanelSlot* BgSlot = Root->AddChildToCanvas(Background))
		{
			BgSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			BgSlot->SetOffsets(FMargin(0.f));
		}

		// Padded content column.
		UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		if (UCanvasPanelSlot* ColSlot = Root->AddChildToCanvas(Column))
		{
			ColSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			ColSlot->SetOffsets(FMargin(56.f, 40.f, 56.f, 40.f));
		}

		// Header: user name (left, fills) + clock (right, accent).
		UHorizontalBox* Header = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (UHorizontalBoxSlot* S = Header->AddChildToHorizontalBox(MakeText(UserName, 22, TextColor, true)))
		{
			S->SetSize(FillSize());
			S->SetVerticalAlignment(VAlign_Center);
		}
		if (UHorizontalBoxSlot* S = Header->AddChildToHorizontalBox(MakeText(ClockLabel, 22, AccentColor, true)))
		{
			S->SetHorizontalAlignment(HAlign_Right);
			S->SetVerticalAlignment(VAlign_Center);
		}
		Column->AddChildToVerticalBox(Header);

		// Accent underline.
		UImage* Accent = MakeColorImage(AccentColor, FVector2D(64.f, 4.f));
		if (UVerticalBoxSlot* S = Column->AddChildToVerticalBox(Accent))
		{
			S->SetHorizontalAlignment(HAlign_Fill);
			S->SetPadding(FMargin(0.f, 10.f, 0.f, 18.f));
		}

		// Swappable views.
		DashboardPanel = BuildDashboard();
		DetailPanel = BuildDetail();
		if (UVerticalBoxSlot* S = Column->AddChildToVerticalBox(DashboardPanel))
		{
			S->SetSize(FillSize());
		}
		if (UVerticalBoxSlot* S = Column->AddChildToVerticalBox(DetailPanel))
		{
			S->SetSize(FillSize());
		}

		ShowDashboard();
	}

	return Super::RebuildWidget();
}

UWidget* UBTTerminalWidget::BuildDashboard()
{
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

	UButton* Tile0 = BuildTile(IconMessages, FText::FromString(TEXT("3")), MessagesLabel);
	UButton* Tile1 = BuildTile(IconFiles, FText::FromString(TEXT("0")), FilesLabel);
	UButton* Tile2 = BuildTile(IconSystem, FText::FromString(TEXT("2")), SystemLabel);

	Tile0->OnClicked.AddDynamic(this, &UBTTerminalWidget::ShowMessages);
	Tile1->OnClicked.AddDynamic(this, &UBTTerminalWidget::ShowFiles);
	Tile2->OnClicked.AddDynamic(this, &UBTTerminalWidget::ShowSystem);

	for (UButton* Tile : { Tile0, Tile1, Tile2 })
	{
		if (UHorizontalBoxSlot* S = Row->AddChildToHorizontalBox(Tile))
		{
			S->SetSize(FillSize());
			S->SetPadding(FMargin(12.f));
		}
	}
	return Row;
}

UWidget* UBTTerminalWidget::BuildDetail()
{
	UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());

	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

	UButton* Back = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	StyleTileButton(Back);
	UHorizontalBox* BackContent = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	if (UHorizontalBoxSlot* S = BackContent->AddChildToHorizontalBox(MakeIcon(IconBack, 26.f)))
	{
		S->SetVerticalAlignment(VAlign_Center);
		S->SetPadding(FMargin(6.f, 4.f, 4.f, 4.f));
	}
	if (UHorizontalBoxSlot* S = BackContent->AddChildToHorizontalBox(MakeText(BackLabel, 18, TextColor)))
	{
		S->SetVerticalAlignment(VAlign_Center);
		S->SetPadding(FMargin(2.f, 4.f, 10.f, 4.f));
	}
	Back->SetContent(BackContent);
	Back->OnClicked.AddDynamic(this, &UBTTerminalWidget::ShowDashboard);
	if (UHorizontalBoxSlot* S = HeaderRow->AddChildToHorizontalBox(Back))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}

	DetailIcon = MakeIcon(nullptr, 36.f);
	if (UHorizontalBoxSlot* S = HeaderRow->AddChildToHorizontalBox(DetailIcon))
	{
		S->SetVerticalAlignment(VAlign_Center);
		S->SetPadding(FMargin(24.f, 0.f, 10.f, 0.f));
	}

	DetailTitle = MakeText(FText::GetEmpty(), 28, TextColor, true);
	if (UHorizontalBoxSlot* S = HeaderRow->AddChildToHorizontalBox(DetailTitle))
	{
		S->SetVerticalAlignment(VAlign_Center);
	}
	Box->AddChildToVerticalBox(HeaderRow);

	DetailBody = MakeText(FText::GetEmpty(), 18, TextColor);
	DetailBody->SetAutoWrapText(true);
	if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(DetailBody))
	{
		S->SetPadding(FMargin(4.f, 24.f, 4.f, 4.f));
		S->SetSize(FillSize());
	}

	return Box;
}

UButton* UBTTerminalWidget::BuildTile(UTexture2D* Icon, const FText& Count, const FText& Label)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	StyleTileButton(Button);

	UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(MakeIcon(Icon, 72.f)))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetPadding(FMargin(0.f, 28.f, 0.f, 8.f));
	}
	if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(MakeText(Count, 44, TextColor, true)))
	{
		S->SetHorizontalAlignment(HAlign_Center);
	}
	if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(MakeText(Label, 16, TextColor)))
	{
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetPadding(FMargin(0.f, 4.f, 0.f, 28.f));
	}
	Button->SetContent(Box);
	return Button;
}

void UBTTerminalWidget::StyleTileButton(UButton* Button)
{
	FButtonStyle Style;
	Style.SetNormal(FSlateColorBrush(FLinearColor(1.f, 1.f, 1.f, 0.55f)));
	Style.SetHovered(FSlateColorBrush(AccentColor.CopyWithNewOpacity(0.22f)));
	Style.SetPressed(FSlateColorBrush(AccentColor.CopyWithNewOpacity(0.38f)));
	Button->SetStyle(Style);
}

UTextBlock* UBTTerminalWidget::MakeText(const FText& Text, int32 Size, const FLinearColor& Color, bool bBold)
{
	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TextBlock->SetText(Text);
	FSlateFontInfo Font = TextBlock->GetFont();
	Font.Size = Size;
	if (bBold)
	{
		Font.TypefaceFontName = FName("Bold");
	}
	TextBlock->SetFont(Font);
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	return TextBlock;
}

UImage* UBTTerminalWidget::MakeColorImage(const FLinearColor& Color, const FVector2D& Size)
{
	UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	FSlateBrush Brush = FSlateColorBrush(Color);
	Brush.ImageSize = Size;
	Image->SetBrush(Brush);
	return Image;
}

UImage* UBTTerminalWidget::MakeIcon(UTexture2D* Texture, float Size)
{
	UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	if (Texture)
	{
		Image->SetBrushFromTexture(Texture, false);
	}
	Image->SetDesiredSizeOverride(FVector2D(Size, Size));
	// White line-icons get tinted to the text colour for contrast on the light background.
	Image->SetColorAndOpacity(TextColor);
	return Image;
}

void UBTTerminalWidget::ShowDetail(const FText& Title, const FText& Body, UTexture2D* Icon)
{
	if (DetailTitle)
	{
		DetailTitle->SetText(Title);
	}
	if (DetailBody)
	{
		DetailBody->SetText(Body);
	}
	if (DetailIcon && Icon)
	{
		DetailIcon->SetBrushFromTexture(Icon, false);
	}
	if (DashboardPanel)
	{
		DashboardPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DetailPanel)
	{
		DetailPanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBTTerminalWidget::ShowMessages()
{
	ShowDetail(MessagesLabel, MessagesBody, IconMessages);
}

void UBTTerminalWidget::ShowFiles()
{
	ShowDetail(FilesLabel, FilesBody, IconFiles);
}

void UBTTerminalWidget::ShowSystem()
{
	ShowDetail(SystemLabel, SystemBody, IconSystem);
}

void UBTTerminalWidget::ShowDashboard()
{
	if (DashboardPanel)
	{
		DashboardPanel->SetVisibility(ESlateVisibility::Visible);
	}
	if (DetailPanel)
	{
		DetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}
