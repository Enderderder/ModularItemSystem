// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemCreator.h"
#include "SlateOptMacros.h"

#include "ModularItemSubsystem.h"
#include "ItemAttributeBase.h"

// Widget build blocks
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Views/SListView.h" // TODO: Maybe remove later
#include "Widgets/Views/STableRow.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"

#include "Widgets/SItemDetailEditor.h"
#include "Widgets/SItemDataListViewRow.h"

#include "PropertyEditorModule.h"
#include "IStructureDetailsView.h"
#include "IDetailsView.h"

#define LOCTEXT_NAMESPACE "SItemCreator"

void SItemCreator::PreChange(const class UUserDefinedStruct* _struct, FStructureEditorUtils::EStructureEditorChangeInfo _info)
{
}

void SItemCreator::PostChange(const class UUserDefinedStruct* _struct, FStructureEditorUtils::EStructureEditorChangeInfo _info)
{
	const UDataTable* table = GetDataTable();
 	if (_struct && table && (table->GetRowStruct() == _struct))
 	{
 		HandlePostChange();
 	}
}

void SItemCreator::PreChange(const UDataTable* _changed, FDataTableEditorUtils::EDataTableChangeInfo _info)
{
}

void SItemCreator::PostChange(const UDataTable* _changed, FDataTableEditorUtils::EDataTableChangeInfo _info)
{
	UDataTable* table = GetEditableDataTable();
	if (_changed == table)
	{
		HandlePostChange();
		table->OnDataTableChanged().Broadcast();
	}
}

void SItemCreator::SelectionChange(const UDataTable* _changed, FName _rowName)
{
	const UDataTable* Table = GetDataTable();
	if (_changed == Table)
	{
		const bool bSelectionChanged = HighlightedItemName != _rowName;
		SetHightlightItem(_rowName);

		if (bSelectionChanged)
		{
			CallbackOnItemHighlighted.ExecuteIfBound(HighlightedItemName);
		}
	}
}

void SItemCreator::Initialize()
{
	// TODO: Delete if not used

}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemCreator::Construct(const FArguments& _inArgs)
{
	TSharedRef<SVerticalBox> ItemListViewContent = CreateItemListViewContentBox();
	CreateItemDetailEditor();
	
	ChildSlot
	[
		SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot().Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SSpacer).Size(FVector2D(0.0f, 10.0f))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SBox).MinDesiredWidth(400.0f).MaxDesiredWidth(600.0f)
					[
						SNew(STextBlock)
						.Text(STRING_TEXT("Items"))
						.Justification(ETextJustify::Center)
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(STRING_TEXT("Preview & Edit"))
					.Justification(ETextJustify::Center)
				]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(SHorizontalBox)

						// Refresh Item List Button
						+ SHorizontalBox::Slot()
						[
							SAssignNew(RefreshItemListButton, SButton)
							.OnClicked(this, &SItemCreator::OnRefreshItemListButtonClicked)
							[
								SNew(STextBlock)
								.Text(STRING_TEXT("Refresh"))
								.Justification(ETextJustify::Center)
							]
						]

						// Add New Item Button
						+ SHorizontalBox::Slot()
						[
							SAssignNew(AddItemButton, SButton)
							.OnClicked(this, &SItemCreator::OnAddItemButtonClicked)
							[
								SNew(STextBlock)
								.Text(STRING_TEXT("Add Item"))
								.Justification(ETextJustify::Center)
							]
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SBox).MinDesiredWidth(400.0f).MaxDesiredWidth(600.0f)
						[
							SNew(SBorder)
							[
								ItemListViewContent
							]
						]
					]
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						ItemDetailEditor.ToSharedRef()
					]
					+ SScrollBox::Slot()
					[
						SAssignNew(DeleteItemButton, SButton)
						.OnClicked(this, &SItemCreator::OnDeleteItemButtonClicked)
						[
							SNew(STextBlock)
							.Text(STRING_TEXT("Delete"))
							.Justification(ETextJustify::Center)
						]
					]
				]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

const UDataTable* SItemCreator::GetDataTable() const
{
	return UModularItemSubsystem::Get()->GetItemDataTable();
}

void SItemCreator::HandlePostChange()
{
	// We need to cache and restore the selection here as RefreshCachedDataTable will re-create the list view items
	const FName cachedSelection = HighlightedItemName;
	HighlightedItemName = NAME_None;
	RefreshCachedItemDataTable(cachedSelection, true/*bUpdateEvenIfValid*/);
}

void SItemCreator::SetHightlightItem(FName _itemName)
{
	if (_itemName == HighlightedItemName) return;

	if (_itemName.IsNone())
	{
		HighlightedItemName = NAME_None;
		ItemListView->ClearSelection();
		HighlighteditemIndex = INDEX_NONE;
	}
	else
	{
		HighlightedItemName = _itemName;

		FItemDataListViewPtr* newItemSelectionPtr = NULL;
		for (HighlighteditemIndex = 0; HighlighteditemIndex < VisibleItems.Num(); ++HighlighteditemIndex)
		{
			if (VisibleItems[HighlighteditemIndex]->RowId == _itemName)
			{
				newItemSelectionPtr = &(VisibleItems[HighlighteditemIndex]);

				break;
			}
		}

		// Syncronize the list view
		if (newItemSelectionPtr)
		{
			ItemListView->SetSelection(*newItemSelectionPtr);
			ItemListView->RequestScrollIntoView(*newItemSelectionPtr);
		}
		else
		{
			ItemListView->ClearSelection();
		}
	}
}

void SItemCreator::CreateItemDetailEditor()
{
	UDataTable* table = GetEditableDataTable();

	SAssignNew(ItemDetailEditor, SItemDetailEditor, table);
	CallbackOnItemHighlighted.BindSP(ItemDetailEditor.ToSharedRef(), &SItemDetailEditor::SelectItem);
	// TODO: Add undo/redo bindings
}

TSharedRef<SVerticalBox> SItemCreator::CreateItemListViewContentBox()
{
	ItemListView = SNew(SListView<FItemDataListViewPtr>)
		.ItemHeight(24)
		.SelectionMode(ESelectionMode::Single)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.ListItemsSource(&VisibleItems)
		.OnGenerateRow(this, &SItemCreator::OnItemRowGenerated)
		.OnSelectionChanged(this, &SItemCreator::OnItemSelectionChanged);

	RefreshCachedItemDataTable();

	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ItemSearchBox, SSearchBox)
			.InitialText(this, &SItemCreator::GetFilterText)
			.OnTextChanged(this, &SItemCreator::OnFilterTextChanged)
			.OnTextCommitted(this, &SItemCreator::OnFilterTextCommitted)
		]
		+SVerticalBox::Slot()
		[
			ItemListView.ToSharedRef()
		];
}

UDataTable* SItemCreator::GetEditableDataTable() const
{
	return UModularItemSubsystem::Get()->GetItemDataTable();
}

void SItemCreator::CacheItemDataTableForEdit(UDataTable* _dataTable, TArray<FItemDataListViewPtr>& _outListViewPtrArray)
{
	if (!_dataTable || !_dataTable->RowStruct)
	{
		_outListViewPtrArray.Empty();
		return;
	}

	TArray<FItemDataListViewPtr> oldDataRows = _outListViewPtrArray;

	// Populate the data array
	_outListViewPtrArray.Reset(_dataTable->GetRowMap().Num());
	int32 index = 0;
	for (auto rowIt = _dataTable->GetRowMap().CreateConstIterator(); rowIt; ++rowIt, ++index)
	{
		FItemDataListViewPtr cachedRowData;

		// Trys to reuse old data if possible
		if (index >= oldDataRows.Num() || oldDataRows[index]->RowId != rowIt->Key)
		{
			cachedRowData = MakeShareable(new FItemListViewData());
			cachedRowData->RowId = rowIt->Key;
		}
		else
		{
			cachedRowData = oldDataRows[index];
		}

		// Always refresh the item data value
		cachedRowData->ItemData = *(reinterpret_cast<FItemData*>(rowIt->Value));

		_outListViewPtrArray.Add(cachedRowData);
	}
}

TSharedRef<ITableRow> SItemCreator::OnItemRowGenerated(FItemDataListViewPtr _item, const TSharedRef<class STableViewBase>& _ownerTable) const
{
	return
		SNew(SItemDataListViewRow, _ownerTable)
		.ItemDataPtr(_item);
}

void SItemCreator::OnItemSelectionChanged(FItemDataListViewPtr _newInSelection, ESelectInfo::Type _selectInfo)
{
	const bool bSelectionChanged = !_newInSelection.IsValid() || _newInSelection->RowId != HighlightedItemName;
	const FName NewRowName = (_newInSelection.IsValid()) ? _newInSelection->RowId : NAME_None;

	SetHightlightItem(NewRowName);

	if (bSelectionChanged)
	{
		CallbackOnItemHighlighted.ExecuteIfBound(HighlightedItemName);
	}
}

FText SItemCreator::GetFilterText() const
{
	return ActiveFilterText;
}

void SItemCreator::OnFilterTextChanged(const FText& _inFilterText)
{
	ActiveFilterText = _inFilterText;
	UpdateVisibleItems();
}

void SItemCreator::OnFilterTextCommitted(const FText& _newText, ETextCommit::Type _commitInfo)
{
	if (_commitInfo == ETextCommit::OnCleared)
	{
		ItemSearchBox->SetText(FText::GetEmpty());
		OnFilterTextChanged(FText::GetEmpty());
	}
}

void SItemCreator::RefreshCachedItemDataTable(const FName _cachedSelection /*= NAME_None*/, const bool _bUpdateEvenIfValid /*= false*/)
{
	UDataTable* dataTable = GetEditableDataTable();

	CacheItemDataTableForEdit(dataTable, AvaliableItems);

	UpdateVisibleItems(_cachedSelection, _bUpdateEvenIfValid);
}

void SItemCreator::UpdateVisibleItems(const FName _cachedSelection /*= NAME_None*/, const bool _bUpdateEvenIfValid /*= false*/)
{
	if (ActiveFilterText.IsEmptyOrWhitespace())
	{
		VisibleItems = AvaliableItems;
	}
	else
	{
		VisibleItems.Empty(AvaliableItems.Num());

		const FString& activeFilterString = ActiveFilterText.ToString();
		for (const FItemDataListViewPtr& itemData : AvaliableItems)
		{
			bool bPassesFilter = false;

			// Filter for item name
			if (itemData->RowId.ToString().Contains(activeFilterString))
			{
				bPassesFilter = true;
			}
			// Filter for item display name
			else if (itemData->ItemData.DisplayName.Contains(activeFilterString))
			{
				bPassesFilter = true;
			}
			// Filter for item attribute names
			else
			{
				for (auto attribute : itemData->ItemData.ItemAttributes)
				{
					if (attribute)
					{
						// Filter for attribute name
						const FName attributeName = attribute->GetFName();
						if (attributeName.ToString().Contains(activeFilterString))
						{
							bPassesFilter = true;
						}
						// Filter for attribute tag
						else
						{
							for (auto tag : attribute->AttributeTags)
							{
								if (tag.ToString().Contains(activeFilterString))
								{
									bPassesFilter = true;
								}
							}
						}
					}
				}
			}

			if (bPassesFilter)
			{
				VisibleItems.Add(itemData);
			}
		}
	}

	ItemListView->RequestListRefresh();
	RestoreCachedSelection(_cachedSelection, _bUpdateEvenIfValid);
}

void SItemCreator::RestoreCachedSelection(const FName _cachedSelection, const bool _bUpdateEvenIfValid /*= false*/)
{
	// Validate the requested selection to see if it matches a known row
	bool bSelectedRowIsValid = false;
	if (!_cachedSelection.IsNone())
	{
		bSelectedRowIsValid = VisibleItems.ContainsByPredicate([&_cachedSelection](const FItemDataListViewPtr& RowData) -> bool
			{
				return RowData->RowId == _cachedSelection;
			});
	}

	// Apply the new selection (if required)
	if (!bSelectedRowIsValid)
	{
		SetHightlightItem((VisibleItems.Num() > 0) ? VisibleItems[0]->RowId : NAME_None);
		CallbackOnItemHighlighted.ExecuteIfBound(HighlightedItemName);
	}
	else if (_bUpdateEvenIfValid)
	{
		SetHightlightItem(_cachedSelection);
		CallbackOnItemHighlighted.ExecuteIfBound(HighlightedItemName);
	}
}

void SItemCreator::CreateNewItem()
{
	if (UDataTable* dataTable = GetEditableDataTable())
	{
		// Creates an non-repeated row name
		FName newItemName = DataTableUtils::MakeValidName(TEXT("NewItem"));
		while (dataTable->GetRowMap().Contains(newItemName))
		{
			newItemName.SetNumber(newItemName.GetNumber() + 1);
		}

		uint8* newRow = FDataTableEditorUtils::AddRow(dataTable, newItemName);
		if (newRow)
		{
			FDataTableEditorUtils::SelectRow(dataTable, newItemName);

			FItemData* newItem = (FItemData*)newRow;
			newItem->ItemName = newItemName;
		}
	}
}

void SItemCreator::RemoveSelectedItem()
{
	if (UDataTable* dataTable = GetEditableDataTable())
	{
		// We must perform this before removing the row
		const int32 itemToRemoveIdex = VisibleItems.IndexOfByPredicate([&](const FItemDataListViewPtr& _inItemName) -> bool
			{
				return _inItemName->RowId == HighlightedItemName;
			});

		// Remove row
		if (FDataTableEditorUtils::RemoveRow(dataTable, HighlightedItemName))
		{
			// Try and keep the same row index selected
			const int32 RowIndexToSelect = FMath::Clamp(itemToRemoveIdex, 0, VisibleItems.Num() - 1);
			if (VisibleItems.IsValidIndex(RowIndexToSelect))
			{
				FDataTableEditorUtils::SelectRow(dataTable, VisibleItems[RowIndexToSelect]->RowId);
			}
			// Refresh list. Otherwise, the removed row would still appear in the screen until the next list refresh. An
			// analog of CellsListView->RequestListRefresh() also occurs inside FDataTableEditorUtils::SelectRow
			else
			{
				ItemListView->RequestListRefresh();
			}
		}
	}
}

FReply SItemCreator::OnRefreshItemListButtonClicked()
{
	RefreshCachedItemDataTable();
	return FReply::Handled();
}

FReply SItemCreator::OnAddItemButtonClicked()
{
	CreateNewItem();

	return FReply::Handled();
}

FReply SItemCreator::OnDeleteItemButtonClicked()
{
	RemoveSelectedItem();
	
	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE