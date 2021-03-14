// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemDataListViewRow.h"
#include "SlateOptMacros.h"

#include "SItemDataCompactRowView.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemDataListViewRow::Construct(const FArguments& _inArgs, const TSharedRef<STableViewBase>& _inOwnerTableView)
{
	ModularItemPtr = _inArgs._ModularItemPtr;
	STableRow<FModularItemListViewPtr>::Construct(
		STableRow<FModularItemListViewPtr>::FArguments(),
		_inOwnerTableView);

	// Build widget content
	ChildSlot
	[
		SAssignNew(CompactItemRowView, SItemDataCompactRowView, ModularItemPtr->ItemData)
		.bUseSmallIcon(_inArgs._bUseSmallIcon)
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
