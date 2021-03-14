// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "ModularItemTypes.h"

/**
 * 
 */
class SItemDataCompactRowView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SItemDataCompactRowView) :
		_bUseSmallIcon(false),
		_bDrawBackground(false)
	{}

	SLATE_ARGUMENT(bool, bUseSmallIcon)

	SLATE_ARGUMENT(bool, bDrawBackground)

	SLATE_END_ARGS()

public:

	/**
	 * Set the content of this widget with an new item
	 */
	void Refresh(const FModularItemData& _newItemData);

private:

	/** Brush resource for item icon */
	TSharedPtr<FSlateBrush>	ItemIconBrush;

	/** Widget references */
	TSharedPtr<SImage>		ItemIconWidget;
	TSharedPtr<STextBlock>	ItemNameWidget;
	TSharedPtr<STextBlock>	ItemDisplayNameWidget;

public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs, const FModularItemData& _itemData);
};
