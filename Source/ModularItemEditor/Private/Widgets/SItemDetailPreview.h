// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "ModularItemTypes.h"

/**
 *
 */
class SItemDetailPreview : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SItemDetailPreview) :
		_CurrentItem(FModularItemData())
	{}

	SLATE_ATTRIBUTE(TOptional<FModularItemData>, CurrentItem)

	SLATE_END_ARGS()

public:

	/**
	 * Set the content of this widget with an new item
	 */
	void Refresh(const FModularItemData& _newItemData);

protected:

	virtual void BuildItemBasicDetail(const FModularItemData& _itemData);

	virtual void CreateDetailsRow(const FText& _rowName, TSharedRef<SWidget> _rowContent);

	virtual void BuildItemAttributeDetail(const FModularItemData& _itemData);

protected:

	TSharedPtr<FSlateBrush> ItemIconBrush;

	/** The current item attribute we modifying, or optaional if we have multiple selected */
	TAttribute<TOptional<FModularItemData>>	CurrentItem;

	TSharedPtr<SVerticalBox> ItemDetailBox;

	TSharedPtr<SVerticalBox> ItemAttributeBox;

public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs);
};