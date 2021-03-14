// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ModularItemTypes.h"
#include "ModularItemSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogModularItemSystem, Log, All);

class UDataTable;
class UItemAttributeBase;

/**
 * Setting object for the modular item system
 */
UCLASS(config = ModularItemSystem, defaultconfig)
class MODULARITEMSYSTEM_API UModularItemSystemSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, meta = (AllowedClasses = "DataTable"))
	FSoftObjectPath ConfigDataTablePath;
};

/**
 * 
 */
UCLASS()
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

	/**
	 * Check if the data table row struct is FModularItemData
	 */
	static bool IsTableUsingModularItemData(UDataTable* _table);

	//~ Begin USubsystem Interface
	void Initialize(FSubsystemCollectionBase& _collection) override;
	//~ End USubsystem Interface

private:

	/**
	 * Called when the Unreal asset registery has loaded all the files
	 */
	void OnAssetFilesLoaded();

public:

	/**
	 * Gets the item data by its' unique id
	 * @param	_itemId  The ID of the item
	 * @return	The item data
	 * 
	 * @note	This can be very slow depends on the size of the item database
	 */
	FModularItemData GetItemById(const FGuid& _itemId) const;

	/**
	 * Gets the item data by its' name
	 * @param _itemName  The name of the item (Not the display name)
	 * @return The item data
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	FModularItemData GetItemByName(const FName& _itemName) const;

	/**
	 * Gets the item's attributes
	 * @param _itemName  The name of the item (Not the display name)
	 * @return Array of attributes
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	TArray<UItemAttributeBase*> GetItemAttributes(const FName& _itemName) const;

	/**
	 * Gets all the items that have the attribute
	 * @param _attributeClass  The class type of the attribute
	 * @return Array of items
	 */
	UFUNCTION(BlueprintCallable, Category = "ModularItemSystem")
	const TArray<FModularItemData> FindItemsContainAttribute(TSubclassOf<UItemAttributeBase> _attributeClass) const;


private:

	/** The user setting */
	//UPROPERTY()
	//UModularItemSystemSettings* MiSettings;

	/** The item data table that will be loaded according to the reference path in runtime */
	UPROPERTY(Transient)
	UDataTable* ItemDataTable;

public:

	/**
	 * Change the data table that we are using
	 * @param _newDataTable  The new data table
	 */
	void ChangeDataTable(UDataTable* _newDataTable);

public:

	FORCEINLINE class UDataTable* GetItemDataTable() const {
		return ItemDataTable;
	}

	FORCEINLINE UModularItemSystemSettings* GetSettings() {
		return Cast<UModularItemSystemSettings>(UModularItemSystemSettings::StaticClass()->GetDefaultObject());
	}

	

};
