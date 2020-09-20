// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ModularItemTypes.h"
#include "ModularItemSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogModularItemSystem, Log, All);

class UItemAttributeBase;

/**
 * 
 */
UCLASS(config=ModularItemSystem, defaultconfig)
class MODULARITEMSYSTEM_API UModularItemSubsystem final : public UEngineSubsystem
{
	GENERATED_BODY()

private:

	/** Context name for warning msgs (mainly for datatable queries) */
	const TCHAR* WarningContext = TEXT("Modular Item System");

public:
	UModularItemSubsystem();

	/**
	 * Gets the subsystem instance
	 * @return UModularItemSubsystem instance (can be nulled)
	 */
	static UModularItemSubsystem* Get();

	//~ Begin USubsystem Interface
	void Initialize(FSubsystemCollectionBase& _collection) override;
	//~ End USubsystem Interface

#if WITH_EDITOR

public:


#endif WITH_EDITOR


private:

	/**
	 * Called when the Unreal asset registery has loaded all the files
	 */
	void OnAssetFilesLoaded();

	/**
	 * Load data array from the data table
	 * @note This will only success if the "LoadedItemDataArray" is empty
	 */
	void LoadItemData();

public:

	/**
	 * Gets the item data by its' name
	 * @param _itemName The name of the item (Not the display name)
	 * @return The item data
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	FItemData GetItemByName(const FName& _itemName) const;

	/**
	 * Gets the item's attributes
	 * @param _itemName The name of the item (Not the display name)
	 * @return Array of attributes
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	TArray<UItemAttributeBase*> GetItemAttributes(FName _itemName) const;

	/**
	 * Gets all the items that have the attribute
	 * @param _attributeClass The class type of the attribute
	 * @return Array of items
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	const TArray<FItemData> FindItemsContainAttribute(TSubclassOf<class UItemAttributeBase> _attributeClass) const;


private:

	/** The item data table that will be loaded according to the reference path in runtime */
	UPROPERTY(Transient)
	class UDataTable* ItemDataTable;

	/** The item datas that is currently loaded, might differ with the actual data table in real-time */
	UPROPERTY(Transient)
	TArray<FItemData> LoadedItemDataArray;

public:

	FORCEINLINE class UDataTable* GetItemDataTable() const {
		return ItemDataTable;
	}
};
