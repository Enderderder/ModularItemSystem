// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

#include "ModularItemTypes.h"

/**
 * 
 */
class SItemDataListViewRow : public STableRow<FItemDataListViewPtr>
{
public:
	SLATE_BEGIN_ARGS(SItemDataListViewRow){}
	/** The item row we're working with to allow us to get naming information. */
	SLATE_ARGUMENT(FItemDataListViewPtr, ItemDataPtr)
	SLATE_END_ARGS()

protected:

	const FSlateBrush* GetItemIconBrush();


private:

	FItemDataListViewPtr ItemDataPtr;

	TSharedPtr<STableRow<FItemDataListViewPtr>> ItemTableRow;

public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs, const TSharedRef<STableViewBase>& _inOwnerTableView);
};
