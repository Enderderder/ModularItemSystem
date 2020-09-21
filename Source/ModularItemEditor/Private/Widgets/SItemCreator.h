// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "Misc/NotifyHook.h"
#include "Kismet2/StructureEditorUtils.h"
#include "DataTableEditorUtils.h"

#include "ModularItemTypes.h"

// Define string to text macro
#define STRING_TEXT(Content) FText::FromString(##Content)

DECLARE_DELEGATE_OneParam(FOnItemHighlighted, FName /*Item name*/);

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

	/**
	 * Initialize function, called by module tab spawner
	 */
	void Initialize();

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
	TSharedPtr<class SListView<FItemDataListViewPtr>> ItemListView;

	/** The items that are displaying on the item list */
	TArray<FItemDataListViewPtr> VisibleItems;

	/** The items that are avaliable from the data table */
	TArray<FItemDataListViewPtr> AvaliableItems;

	/** Item thats being highlighted/selected */
	FName HighlightedItemName;
	int32 HighlighteditemIndex;

	/** The UI of the item list view search box */
	TSharedPtr<class SSearchBox> ItemSearchBox;

	/** The fitler text for the item search box */
	FText ActiveFilterText;


	TSharedPtr<class SItemDetailEditor> ItemDetailEditor;

	TSharedPtr<class SHorizontalBox> PreviewEditBtnHodler;

protected:

	/**
	 * 
	 */
	void OnMiSettingsChanged(const FPropertyChangedEvent& _event);

	/**
	 * 
	 */
	void CacheItemDataTableForEdit(UDataTable* _dataTable, TArray<FItemDataListViewPtr>& _outListViewPtrArray);

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
	TSharedRef<class ITableRow> OnItemRowGenerated(FItemDataListViewPtr _item, const TSharedRef<class STableViewBase>& _ownerTable) const;

	/**
	 * Called when the selection changed with in the list view
	 * @param _item The item pointer that gets selected
	 * @param _selectInfo Selection info
	 */
	void OnItemSelectionChanged(FItemDataListViewPtr _item, ESelectInfo::Type _selectInfo);

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


#pragma region Button Binding

private:

	TSharedPtr<class SButton> AddItemButton;
	FReply OnAddItemButtonClicked();

	TSharedPtr<class SButton> DeleteItemButton;
	FReply OnDeleteItemButtonClicked();

#pragma endregion Button Binding


};
