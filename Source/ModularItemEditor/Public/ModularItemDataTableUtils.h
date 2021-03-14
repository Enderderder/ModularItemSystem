// Fill out your copyright notice in the Description page of Project Settings.

#pragma  once

#include "CoreMinimal.h"

#include "ModularItemTypes.h"

/**
 * 
 */
struct FModularItemListViewData
{
	/** Unique FName ID used to identify this row */
	FName RowName;

	/** Insertion number of the row */
	uint32 RowNum;

	/** Struct data of the item */
	FModularItemData ItemData;
};

typedef TSharedPtr<FModularItemListViewData> FModularItemListViewPtr;

/**
 * Utility class for modular item data editors
 */
struct FModularItemEditorUtils
{

	/**
	 * Cache item datas from a data table or the binded item data table from modular item subsystem
	 */
	static void CacheItemDataTable(TArray<FModularItemListViewPtr>& _outputArray);
	static void CacheItemDataTable(const UDataTable* _dataTable, TArray<FModularItemListViewPtr>& _outputArray);

	/**
	 * Search filter for modular item list
	 */
	static void ModularItemFilterPass(const FText& _filterText, const TArray<FModularItemListViewPtr>& _avaliableItemPtrs, TArray<FModularItemListViewPtr>& _resultItemPtrs);


};