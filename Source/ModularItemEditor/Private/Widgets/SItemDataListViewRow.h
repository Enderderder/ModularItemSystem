// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

#include "ModularItemTypes.h"


class SItemDataCompactRowView;

/**
 * 
 */
class SItemDataListViewRow : public STableRow<FModularItemListViewPtr>
{
public:
	SLATE_BEGIN_ARGS(SItemDataListViewRow) :
		_bUseSmallIcon(false)
	{}
	
	/** The item row we're working with to allow us to get naming information. */
	SLATE_ARGUMENT(FModularItemListViewPtr, ModularItemPtr)
	
	SLATE_ARGUMENT(bool, bUseSmallIcon)
	
	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs, const TSharedRef<STableViewBase>& _inOwnerTableView);

private:

	/** Item we displaying */
	FModularItemListViewPtr ModularItemPtr;

	/** Display widget for the item row */
	TSharedPtr<SItemDataCompactRowView> CompactItemRowView;

};
