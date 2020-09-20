// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemDataListViewRow.h"
#include "SlateOptMacros.h"

#include "Widgets/Views/SListView.h"
#include "Widgets/Images/SImage.h"

// Easy way to set the icon size
#define ITEM_ICON_SIZE 60.0f


const FSlateBrush* SItemDataListViewRow::GetItemIconBrush()
{
	if (ItemDataPtr.IsValid())
	{
		FSlateBrush* brush = new FSlateBrush();
		brush->SetResourceObject(ItemDataPtr->ItemData.Icon);
		return brush;
	}
	
	return nullptr;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemDataListViewRow::Construct(const FArguments& _inArgs, const TSharedRef<STableViewBase>& _inOwnerTableView)
{
	ItemDataPtr = _inArgs._ItemDataPtr;
	STableRow<FItemDataListViewPtr>::Construct(
		STableRow<FItemDataListViewPtr>::FArguments(),
		_inOwnerTableView);

	// Build icon widget
	TSharedRef<SImage> itemIconWidget = SNew(SImage);
	{
		TAttribute<const FSlateBrush*> itemIconAttr =
			TAttribute<const FSlateBrush*>::Create(
				TAttribute<const FSlateBrush*>::FGetter::CreateRaw(this, &SItemDataListViewRow::GetItemIconBrush));
		itemIconWidget->SetImage(itemIconAttr);
	}
	
	// Build widget content
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot() .HAlign(HAlign_Left) .VAlign(VAlign_Center) .AutoWidth()
		[
			SNew(SBox)
			.Padding(10.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.WidthOverride(FOptionalSize(ITEM_ICON_SIZE))
			.HeightOverride(FOptionalSize(ITEM_ICON_SIZE))
			[
				itemIconWidget
			]
		]
		+SHorizontalBox::Slot() 
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromName(ItemDataPtr->RowId))
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
