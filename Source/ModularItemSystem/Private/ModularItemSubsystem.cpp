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

UModularItemSubsystem::UModularItemSubsystem() : UEngineSubsystem()
{
	// Locate the data table in the content folder
	static ConstructorHelpers::FObjectFinder<UDataTable> itemDataTableObject(ITEM_DATA_TABLE_PATH);
	if (itemDataTableObject.Succeeded())
	{
		ItemDataTable = itemDataTableObject.Object;
	}
	else
	{
		

		UE_LOG(LogModularItemSystem, Error,
			TEXT("Cannot locate itemDataTableObject, check plugin integrity"));
	}
}

UModularItemSubsystem* UModularItemSubsystem::Get()
{
	if (!GEngine) return nullptr;
	return GEngine->GetEngineSubsystem<UModularItemSubsystem>();
}

void UModularItemSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	Super::Initialize(_collection);

	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();
	// Must wait until all assets are discovered
	if (assetRegistry.IsLoadingAssets()) {
		assetRegistry.OnFilesLoaded().AddUObject(this, &UModularItemSubsystem::OnAssetFilesLoaded);
	}
	else OnAssetFilesLoaded();

	LoadItemData();
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

#if WITH_EDITOR

// TODO: Potential editor code?

#endif WITH_EDITOR

// TODO: use data table straight instead
void UModularItemSubsystem::LoadItemData()
{
	if (LoadedItemDataArray.Num() != 0) return;

	// Loads the data table into an array
	TArray<FItemData*> outputDataArray;
	ItemDataTable->GetAllRows<FItemData>(TEXT("ModularItemSystem"), outputDataArray);
}

TArray<UItemAttributeBase*> UModularItemSubsystem::GetItemAttributes(FName _itemName) const
{
	TArray<UItemAttributeBase*> outputData;
	if (ItemDataTable)
	{
		FItemData* itemData = ItemDataTable->FindRow<FItemData>(_itemName, WarningContext);
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

const TArray<FItemData> UModularItemSubsystem::FindItemsContainAttribute(TSubclassOf<class UItemAttributeBase> _attributeClass) const
{
	TArray<FItemData> outputData;
	if (ItemDataTable)
	{
		TArray<FItemData*> itemDatas;
		ItemDataTable->GetAllRows(WarningContext, itemDatas);

		for (FItemData* item : itemDatas)
		{
			if (item->ItemAttributeTypes.Contains(_attributeClass))
			{
				outputData.Add(*item);
			}
		}
	}

	return outputData;
}

FItemData UModularItemSubsystem::GetItemByName(const FName& _itemName) const
{
	if (ItemDataTable)
	{
		FItemData* itemData = ItemDataTable->FindRow<FItemData>(_itemName, WarningContext);
		return itemData ? *itemData : FItemData();
	}

	// returns an empty struct
	return FItemData();
}
