// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularItemDataTableUtils.h"
#include "Engine/DataTable.h"

#include "ModularItemSubsystem.h"

void FModularItemEditorUtils::CacheItemDataTable(TArray<FModularItemListViewPtr>& _outputArray)
{
	const UDataTable* miDataTable = UModularItemSubsystem::Get()->GetItemDataTable();

	CacheItemDataTable(miDataTable, _outputArray);
}

void FModularItemEditorUtils::CacheItemDataTable(const UDataTable* _dataTable, TArray<FModularItemListViewPtr>& _outputArray)
{
	if (!_dataTable || !_dataTable->RowStruct)
	{
		_outputArray.Empty();
		return;
	}

	TArray<FModularItemListViewPtr> oldDataRows = _outputArray;

	// Populate the data array
	_outputArray.Reset(_dataTable->GetRowMap().Num());
	int32 index = 0;
	for (auto rowIt = _dataTable->GetRowMap().CreateConstIterator(); rowIt; ++rowIt, ++index)
	{
		FModularItemListViewPtr cachedRowData;

		// Try to reuse old data if possible
		if (index >= oldDataRows.Num() || oldDataRows[index]->RowName != rowIt->Key)
		{
			cachedRowData = MakeShareable(new FModularItemListViewData());
			cachedRowData->RowName = rowIt->Key;
		}
		else
		{
			cachedRowData = oldDataRows[index];
		}

		// Always refresh the item data value
		cachedRowData->ItemData = *(reinterpret_cast<FModularItemData*>(rowIt->Value));

		_outputArray.Add(cachedRowData);
	}
}

void FModularItemEditorUtils::ModularItemFilterPass(const FText& _filterText, const TArray<FModularItemListViewPtr>& _avaliableItemPtrs, TArray<FModularItemListViewPtr>& _resultItemPtrs)
{
	if (_filterText.IsEmptyOrWhitespace())
	{
		_resultItemPtrs = _avaliableItemPtrs;
	}
	else
	{
		_resultItemPtrs.Empty(_avaliableItemPtrs.Num());

		const FString& filterString = _filterText.ToString();
		for (const auto& itemDataPtr : _avaliableItemPtrs)
		{
			bool bPassesFilter = false;

			// Filter for item name
			if (itemDataPtr->RowName.ToString().Contains(filterString))
			{
				bPassesFilter = true;
			}
			// Filter for item display name
			else if (itemDataPtr->ItemData.DisplayName.Contains(filterString))
			{
				bPassesFilter = true;
			}
			// Filter for item attribute names
			else
			{
				for (const auto attribute : itemDataPtr->ItemData.ItemAttributes)
				{
					if (attribute)
					{
						// Filter for attribute name
						const FName attributeName = attribute->GetFName();
						if (attributeName.ToString().Contains(filterString))
						{
							bPassesFilter = true;
						}
						// Filter for attribute tag
						else
						{
							for (const auto tag : attribute->AttributeTags)
							{
								if (tag.ToString().Contains(filterString))
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
				_resultItemPtrs.Add(itemDataPtr);
			}
		}
	}
}
