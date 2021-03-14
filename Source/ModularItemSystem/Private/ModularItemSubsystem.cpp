// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularItemSubsystem.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"
#include "DataTableUtils.h"

#include "Engine.h"
#include "Engine/AssetManager.h"

#include "ItemAttributeBase.h"

DEFINE_LOG_CATEGORY(LogModularItemSystem);

/** This path is build-in for the plugin, no change should needed */
#define ITEM_DATA_TABLE_PATH TEXT("DataTable'/ModularItemSystem/DT_ItemDatas.DT_ItemDatas'")

//----------------------------------------------------------------------//
// UModularItemSubsystem
//----------------------------------------------------------------------//
UModularItemSubsystem::UModularItemSubsystem() : UEngineSubsystem()
{
}

UModularItemSubsystem* UModularItemSubsystem::Get()
{
	if (!GEngine) return nullptr;
	return GEngine->GetEngineSubsystem<UModularItemSubsystem>();
}

bool UModularItemSubsystem::IsTableUsingModularItemData(UDataTable* _table)
{
	if (!_table) return false;

	return (_table->GetRowStruct() == FModularItemData::StaticStruct()) ? true : false;
}

void UModularItemSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	Super::Initialize(_collection);

	// Try to load the data table from setting
	auto tableObject = GetSettings()->ConfigDataTablePath.ResolveObject();
	if (!tableObject)
		tableObject = GetSettings()->ConfigDataTablePath.TryLoad();
	ItemDataTable = Cast<UDataTable>(tableObject);

	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();
	// Must wait until all assets are discovered
	if (assetRegistry.IsLoadingAssets()) {
		assetRegistry.OnFilesLoaded().AddUObject(this, &UModularItemSubsystem::OnAssetFilesLoaded);
	}
	else OnAssetFilesLoaded();
}

void UModularItemSubsystem::OnAssetFilesLoaded()
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();

	// Find all the asset under this class
	FARFilter filter;
	filter.ClassNames = { TEXT("ItemAttributeBase") };
	filter.bRecursiveClasses = true;

// 	TArray<FAssetData> assetList;
// 	assetRegistry.GetAssets(filter, assetList);
// 
// 	TArray<UItemDataAssetBase*> result;
// 	for (const FAssetData& asset : assetList)
// 	{
// 		UObject* assetObj = asset.GetAsset();
// 		if (assetObj->GetClass()->IsChildOf(UItemDataAssetBase::StaticClass()))
// 		{
// 			// TODO: Save those asset reference somewhere (Or not?)
// 		}
// 	}
}

TArray<UItemAttributeBase*> UModularItemSubsystem::GetItemAttributes(const FName& _itemName) const
{
	TArray<UItemAttributeBase*> outputData;
	if (ItemDataTable)
	{
		FModularItemData* itemData = ItemDataTable->FindRow<FModularItemData>(_itemName, WarningContext);
		if (itemData)
		{
			for (const auto itemAttr : itemData->ItemAttributes)
			{
				outputData.Add(itemAttr);
			}
		}
	}

	return outputData;
}

const TArray<FModularItemData> UModularItemSubsystem::FindItemsContainAttribute(TSubclassOf<UItemAttributeBase> _attributeClass) const
{
	TArray<FModularItemData> outputData;
	if (ItemDataTable)
	{
		TArray<FModularItemData*> itemDatas;
		ItemDataTable->GetAllRows(WarningContext, itemDatas);

		for (FModularItemData* item : itemDatas)
		{
			if (item->ItemAttributeTypes.Contains(_attributeClass))
			{
				outputData.Add(*item);
			}
		}
	}

	return outputData;
}

FModularItemData UModularItemSubsystem::GetItemById(const FGuid& _itemId) const
{
	if (ItemDataTable)
	{
		TArray<FModularItemData*> itemRows;
		ItemDataTable->GetAllRows(WarningContext, itemRows);

		for (const auto* item : itemRows)
		{
			if (item->GetItemId() == _itemId)
				return *item;
		}
	}

	return FModularItemData();
}

FModularItemData UModularItemSubsystem::GetItemByName(const FName& _itemName) const
{
	if (ItemDataTable)
	{
		FModularItemData* itemData = ItemDataTable->FindRow<FModularItemData>(_itemName, WarningContext);
		return itemData ? *itemData : FModularItemData();
	}

	// returns an empty struct if not found
	return FModularItemData();
}

void UModularItemSubsystem::ChangeDataTable(UDataTable* _newDataTable)
{
	if (IsTableUsingModularItemData(_newDataTable))
	{
		ItemDataTable = _newDataTable;
	}
}
