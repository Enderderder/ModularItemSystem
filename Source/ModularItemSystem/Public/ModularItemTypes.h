// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Uobject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "ModularItemTypes.generated.h"

/**
 * This struct holds the data/information of an item
 */
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Icon;

	// This will sync with the datatable row name, cannot be deplicated
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	// This can be duplicated
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FString Description;

	// 0 means infinite stack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<class UItemAttributeBase>> ItemAttributeTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class UItemAttributeBase*> ItemAttributes;

	FItemData() :
		Id(0),
		Icon(nullptr),
		ItemName(TEXT("")),
		DisplayName(TEXT("")),
		Description(TEXT("")),
		MaxStack(0)
	{}
};

struct FItemListViewData
{
	/** Unique ID used to identify this row */
	FName RowId;

	/** Insertion number of the row */
	uint32 RowNum;

	/** Struct data of the item */
	FItemData ItemData;
};


typedef TSharedPtr<FItemListViewData> FItemDataListViewPtr;

