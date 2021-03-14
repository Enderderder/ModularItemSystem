// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularItemTypes.h"

/** Forward declare */
class UItemAttributeBase;

/**
 * 
 */
struct MODULARITEMSYSTEM_API FModularItemDataUtil
{

	static TArray<UItemAttributeBase*> GetItemAttributes(UDataTable* _dataTable, FName _itemName);





};
