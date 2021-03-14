// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemDataCompactRowView.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"


// Easy way to set the icon size
#define ITEM_ICON_SIZE			69.0f
#define ITEM_ICON_SIZE_SMALL	42.0f

#define LOCTEXT_NAMESPACE "ItemDataCompactRowView"


void SItemDataCompactRowView::Refresh(const FModularItemData& _newItemData)
{
	ItemIconBrush->SetResourceObject(_newItemData.Icon);
	ItemNameWidget->SetText(FText::FromName(_newItemData.ItemName));
	ItemDisplayNameWidget->SetText(FText::FromString(_newItemData.DisplayName));
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemDataCompactRowView::Construct(const FArguments& _inArgs, const FModularItemData& _itemData)
{	
	// Figure out with size of icon we using
	float iconSize = _inArgs._bUseSmallIcon ? ITEM_ICON_SIZE_SMALL : ITEM_ICON_SIZE;

	// Prebuild item data preview component
	{
		ItemIconBrush = MakeShareable(new FSlateBrush);
		ItemIconBrush->SetResourceObject(_itemData.Icon);

		// Create a white text style
		FTextBlockStyle style = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
		style.SetColorAndOpacity(FSlateColor(FLinearColor::White));
		style.Font.Size = 12.0f;
		style.Font.OutlineSettings.OutlineSize = 1.0f;

		SAssignNew(ItemIconWidget, SImage)
			.Image(ItemIconBrush.Get());

		SAssignNew(ItemNameWidget, STextBlock)
			.TextStyle(&style)
			.Text(FText::FromName(_itemData.ItemName));

		SAssignNew(ItemDisplayNameWidget, STextBlock)
			.TextStyle(&style)
			.Text(FText::FromString(_itemData.DisplayName));
	}
	
	// Prebuild background
	TSharedRef<SWidget> rowViewBackground = SNullWidget::NullWidget;
	if (_inArgs._bDrawBackground)
	{
		rowViewBackground = SNew(SImage)
			.Image(new FSlateColorBrush(FColor::Black));
	}

	// Build widget content
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot() 
		.HAlign(HAlign_Fill) 
		.VAlign(VAlign_Fill)
		[
			rowViewBackground
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Fill) 
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.WidthOverride(FOptionalSize(iconSize))
				.HeightOverride(FOptionalSize(iconSize))
				[
					ItemIconWidget.ToSharedRef()
				]
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					ItemNameWidget.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					ItemDisplayNameWidget.ToSharedRef()
				]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE