// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularItemDataUtil.h"

#include "ItemAttributeBase.h"

/** Define log warning context */
#define WARNING_CONTEXT TEXT("Modular Item Utility")

TArray<UItemAttributeBase*> FModularItemDataUtil::GetItemAttributes(UDataTable* _dataTable, FName _itemName)
{
	TArray<UItemAttributeBase*> outputArray;

	FModularItemData* itemData = _dataTable->FindRow<FModularItemData>(_itemName, WARNING_CONTEXT);
	if (itemData)
	{
		for (const auto itemAttr : itemData->ItemAttributes)
		{
			outputArray.Add(itemAttr);
		}
	}

	return outputArray;
}
