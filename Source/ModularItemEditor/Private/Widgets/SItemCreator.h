// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "Misc/NotifyHook.h"
#include "Kismet2/StructureEditorUtils.h"
#include "DataTableEditorUtils.h"
#include "ModularItemDataTableUtils.h"

#include "ModularItemTypes.h"

DECLARE_DELEGATE_OneParam(FOnItemHighlighted, FName /*Item name*/);

class SItemDetailEditor;


/**
 * A widget for item creation
 */
class SItemCreator : public SCompoundWidget
	, public FNotifyHook
	, public FStructureEditorUtils::INotifyOnStructChanged
	, public FDataTableEditorUtils::INotifyOnDataTableChanged
{
public:	
	SLATE_BEGIN_ARGS(SItemCreator){}
	SLATE_END_ARGS()

	//~ Begin INotifyOnStructChanged interface
	virtual void PreChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;
	virtual void PostChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;
	//~ End INotifyOnStructChanged interface

	//~ Begin INotifyOnDataTableChanged interface
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void SelectionChange(const UDataTable* Changed, FName RowName) override;
	//~ End INotifyOnDataTableChanged interface

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs);

	/**
	 * Get the data table we are editing
	 * @return UDatatable
	 */
	const UDataTable* GetDataTable() const;

	/**
	 * Called after data table/struct change
	 */
	void HandlePostChange();

	/**
	 * Highlight the selected item
	 * @param _itemName The name(Non duplicated name) of the item
	 */
	void SetHightlightItem(FName _itemName);


protected:

	void CreateItemDetailEditor();

	TSharedRef<SVerticalBox> CreateItemListViewContentBox();

private:
	UDataTable* GetEditableDataTable() const;

protected:

	FOnItemHighlighted CallbackOnItemHighlighted;

	/** The list view widget for the items */
	TSharedPtr<SListView<FModularItemListViewPtr>> ItemListView;

	/** The items that are avaliable from the data table */
	TArray<FModularItemListViewPtr> AvaliableItems;
	/** The items that are displaying on the item list */
	TArray<FModularItemListViewPtr> VisibleItems;

	/** Item thats being highlighted/selected */
	FName HighlightedItemName;
	int32 HighlighteditemIndex;

	/** The UI of the item list view search box */
	TSharedPtr<SSearchBox> ItemSearchBox;

	/** The fitler text for the item search box */
	FText ActiveFilterText;


	TSharedPtr<SItemDetailEditor> ItemDetailEditor;

	TSharedPtr<SHorizontalBox> PreviewEditBtnHodler;

protected:

	/**
	 * 
	 */
	void OnMiSettingsChanged(const FPropertyChangedEvent& _event);

	/**
	 * Refresh the cached item data table
	 */
	void RefreshCachedItemDataTable(const FName _cachedSelection = NAME_None, const bool _bUpdateEvenIfValid = false);

	/**
	 *
	 */
	void UpdateVisibleItems(const FName _cachedSelection = NAME_None, const bool _bUpdateEvenIfValid = false);

	/**
	 * 
	 */
	void RestoreCachedSelection(const FName _cachedSelection, const bool _bUpdateEvenIfValid = false);

	/**
	 * Called when list view generates an item
	 * @param _item The item pointer that generated
	 * @param _ownerTable The owner widget
	 * @return A shared reference of the table row
	 */
	TSharedRef<class ITableRow> OnItemRowGenerated(FModularItemListViewPtr _item, const TSharedRef<class STableViewBase>& _ownerTable) const;

	/**
	 * Called when the selection changed with in the list view
	 * @param _item The item pointer that gets selected
	 * @param _selectInfo Selection info
	 */
	void OnItemSelectionChanged(FModularItemListViewPtr _item, ESelectInfo::Type _selectInfo);

	/**
	 * @return The filter text for the search box
	 */
	FText GetFilterText() const;

	/**
	 * 
	 */
	void OnFilterTextChanged(const FText& _inFilterText);

	/**
	 * 
	 */
	void OnFilterTextCommitted(const FText& _newText, ETextCommit::Type _commitInfo);

	/**
	 * 
	 */
	void CreateNewItem();

	/**
	 * 
	 */
	void RemoveSelectedItem();

	/**
	 * Callback for when item detail has been modified
	 */
	void OnItemDetailChanged(const FPropertyChangedEvent& _propertyChangedEvent);

#pragma region Button Binding

private:

	TSharedPtr<class SButton> AddItemButton;
	FReply OnAddItemButtonClicked();

	TSharedPtr<class SButton> DeleteItemButton;
	FReply OnDeleteItemButtonClicked();

#pragma endregion Button Binding


};
