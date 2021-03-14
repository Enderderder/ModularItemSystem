// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "ModularItemDataTableUtils.h"

DECLARE_DELEGATE_OneParam(FOnItemChanged, const FModularItemData&)

typedef SListView<FModularItemListViewPtr> SModularItemListView;

class SItemDataCompactRowView;

/**
 * A widget for custom combo box
 */
class SModularItemSelector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SModularItemSelector) :
		_CurrentItem(FModularItemData()),
		_ListViewWidth(300.0f),
		_ListViewHeight(500.0f)
	{}

	SLATE_ATTRIBUTE(TOptional<FModularItemData>, CurrentItem)

	SLATE_EVENT(FOnItemChanged, OnItemChanged)
	
	SLATE_ARGUMENT(FOptionalSize, ListViewWidth)
	SLATE_ARGUMENT(FOptionalSize, ListViewHeight)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs);

public:

	/**
	 * Select the item
	 * @param _itemData
	 */
	virtual void SelectItem(const FModularItemData& _itemData);

protected:

	/** Gets the content of the selector combo button */
	virtual void RebuildComboButtonContent();

	/** Gets the currently selected item data */
	virtual FModularItemData GetCurrentItemData() const;

	/** Gets the content of the selector menu */
	virtual TSharedRef<SWidget> GetItemMenuContent();

	/** Item list view function callbacks */
	virtual TSharedRef<ITableRow> OnGenerateItemRow(FModularItemListViewPtr _itemPtr, const TSharedRef<STableViewBase>& _ownerList);
	void OnItemSelectionChanged(FModularItemListViewPtr _itemPtr, ESelectInfo::Type _selectInfo);

	/** Item list view search function callback */
	void OnFilterTextChanged(const FText& _newText);

	/** Selection clear button callback */
	virtual void OnSelectionCleared();

	/** Recache the avaliable items from the database */
	void RefreshCachedItemList();

	/** update the visible item in the list view upon database change or search filter change */
	void UpdateVisibleItems();

protected:

	/** Combo button that shows shows the current selected button */
	TSharedPtr<SComboButton>				ItemSelectComboButton;
	TSharedPtr<SBox>						ComboButtonContent;
	TSharedPtr<SItemDataCompactRowView>		ItemCompactView;
	TSharedPtr<STextBlock>					MultiItemWarningTextBlock;

	/** Menu content reference for combo box popup */
	TSharedPtr<SWidget>						MenuContent;
	TSharedPtr<SSearchBox>					SearchFilterBox;
	TSharedPtr<SModularItemListView>		ItemListView;
	FText									SearchText;

	/** The current item attribute we modifying, or optaional if we have multiple selected */
	TAttribute<TOptional<FModularItemData>>	CurrentItem;
	
	/** Delegate called when the item selection has been changed */
	FOnItemChanged							OnItemChanged;

	/** The size override of the item drop down selector */
	FOptionalSize							ListViewWidth;
	FOptionalSize							ListViewHeight;

	/** Array containing all the avaliable item to view */
	TArray<FModularItemListViewPtr>			AvaliableItems;
	/** Array containing all the visible item after filtering with search text */
	TArray<FModularItemListViewPtr>			VisibleItems;

};