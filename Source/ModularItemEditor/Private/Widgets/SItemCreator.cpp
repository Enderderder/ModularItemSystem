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
#include "ISinglePropertyView.h"

#define LOCTEXT_NAMESPACE "ItemCreator"

// INotifyOnStructChanged
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

// INotifyOnDataTableChanged
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

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemCreator::Construct(const FArguments& _inArgs)
{
	TSharedRef<SVerticalBox> ItemListViewContent = CreateItemListViewContentBox();
	CreateItemDetailEditor();
	
	FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs viewArgs;
	viewArgs.bAllowSearch = false;
	viewArgs.bHideSelectionTip = false;
	viewArgs.bShowScrollBar = false;
	viewArgs.bShowActorLabel = false;
	viewArgs.NotifyHook = this;

	auto miSystem = UModularItemSubsystem::Get();
	auto miSettingView = propertyModule.CreateDetailView(viewArgs);
	miSettingView->SetObject(miSystem->GetSettings());
	miSettingView->OnFinishedChangingProperties().AddSP(this, &SItemCreator::OnMiSettingsChanged);

	ChildSlot
	[
		SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot().Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot() .AutoHeight()
				[
					miSettingView
				]
				+ SVerticalBox::Slot()
				[
					SNew(SBox).MinDesiredWidth(600.0f).MaxDesiredWidth(800.0f)
					[
						SNew(SBorder)
						[
							ItemListViewContent
						]
					]
				]
				// Add New Item Button
				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(AddItemButton, SButton)
					.OnClicked(this, &SItemCreator::OnAddItemButtonClicked)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Additem", "Add Item"))
						.Justification(ETextJustify::Center)
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
						.Text(LOCTEXT("Delete", "Delete"))
						.Justification(ETextJustify::Center)
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

		FModularItemListViewPtr* newItemSelectionPtr = NULL;
		for (HighlighteditemIndex = 0; HighlighteditemIndex < VisibleItems.Num(); ++HighlighteditemIndex)
		{
			if (VisibleItems[HighlighteditemIndex]->RowName == _itemName)
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
	ItemListView = SNew(SListView<FModularItemListViewPtr>)
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

void SItemCreator::OnMiSettingsChanged(const FPropertyChangedEvent& _event)
{
	const auto	cachedDataTablePath	= GetEditableDataTable()->GetPathName();

	auto		miSystem			= UModularItemSubsystem::Get();

	bool		bNeedToRevertChange = false;

	auto		newItemTableObject	= miSystem->GetSettings()->ConfigDataTablePath.ResolveObject();
	auto		newItemTable		= Cast<UDataTable>(newItemTableObject);
	
	if (!newItemTable ||
		!UModularItemSubsystem::IsTableUsingModularItemData(newItemTable))
	{
		bNeedToRevertChange = true;

		// The item data table is invalid
		// Popup an error dialog here
		const FText Message = LOCTEXT(
			"InvalidItemDataTable",
			"Cannot use this asset as an editable item data table, make sure the row struct of the table is FModularItemData");
		FMessageDialog::Open(EAppMsgType::Ok, Message);
	}

	if (bNeedToRevertChange)
	{
		miSystem->GetSettings()->ConfigDataTablePath.SetPath(cachedDataTablePath);
	}
	else
	{
		miSystem->GetSettings()->SaveConfig();
		miSystem->ChangeDataTable(newItemTable);
		ItemDetailEditor->ChangeDataTable(newItemTable);
		FDataTableEditorUtils::BroadcastPostChange(newItemTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	}
}

TSharedRef<ITableRow> SItemCreator::OnItemRowGenerated(FModularItemListViewPtr _item, const TSharedRef<class STableViewBase>& _ownerTable) const
{
	return
		SNew(SItemDataListViewRow, _ownerTable)
		.ModularItemPtr(_item);
}

void SItemCreator::OnItemSelectionChanged(FModularItemListViewPtr _newInSelection, ESelectInfo::Type _selectInfo)
{
	const bool bSelectionChanged = !_newInSelection.IsValid() || _newInSelection->RowName != HighlightedItemName;
	const FName NewRowName = (_newInSelection.IsValid()) ? _newInSelection->RowName : NAME_None;

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
	FModularItemEditorUtils::CacheItemDataTable(AvaliableItems);

	UpdateVisibleItems(_cachedSelection, _bUpdateEvenIfValid);
}

void SItemCreator::UpdateVisibleItems(const FName _cachedSelection /*= NAME_None*/, const bool _bUpdateEvenIfValid /*= false*/)
{
	FModularItemEditorUtils::ModularItemFilterPass(ActiveFilterText, AvaliableItems, VisibleItems);

	ItemListView->RebuildList();
	//ItemListView->RequestListRefresh();
	RestoreCachedSelection(_cachedSelection, _bUpdateEvenIfValid);
}

void SItemCreator::RestoreCachedSelection(const FName _cachedSelection, const bool _bUpdateEvenIfValid /*= false*/)
{
	// Validate the requested selection to see if it matches a known row
	bool bSelectedRowIsValid = false;
	if (!_cachedSelection.IsNone())
	{
		bSelectedRowIsValid = VisibleItems.ContainsByPredicate([&_cachedSelection](const FModularItemListViewPtr& RowData) -> bool
			{
				return RowData->RowName == _cachedSelection;
			});
	}

	// Apply the new selection (if required)
	if (!bSelectedRowIsValid)
	{
		SetHightlightItem((VisibleItems.Num() > 0) ? VisibleItems[0]->RowName : NAME_None);
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

			FModularItemData* newItem = (FModularItemData*)newRow;
			newItem->ItemName = newItemName;
			newItem->InitItemId();
		}
	}
}

void SItemCreator::RemoveSelectedItem()
{
	if (UDataTable* dataTable = GetEditableDataTable())
	{
		// We must perform this before removing the row
		const int32 itemToRemoveIdex = VisibleItems.IndexOfByPredicate([&](const FModularItemListViewPtr& _inItemName) -> bool
			{
				return _inItemName->RowName == HighlightedItemName;
			});

		// Remove row
		if (FDataTableEditorUtils::RemoveRow(dataTable, HighlightedItemName))
		{
			// Try and keep the same row index selected
			const int32 RowIndexToSelect = FMath::Clamp(itemToRemoveIdex, 0, VisibleItems.Num() - 1);
			if (VisibleItems.IsValidIndex(RowIndexToSelect))
			{
				FDataTableEditorUtils::SelectRow(dataTable, VisibleItems[RowIndexToSelect]->RowName);
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