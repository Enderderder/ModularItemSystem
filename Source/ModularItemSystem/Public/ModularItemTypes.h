// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DataTable.h"
#include "ModularItemTypes.generated.h"

/**
 * This struct holds the data/information of an item
 */
USTRUCT(BlueprintType)
struct FModularItemData : public FTableRowBase
{
	GENERATED_BODY()

protected:

	// The never change id when the item is created
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid Id;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Icon;

	// This will sync with the datatable row name, cannot be duplicated
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	// This can be duplicated
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInifiniteStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, EditCondition = "!bInifiniteStack"))
	int32 MaxStack;

	// Deprecated as this is unnecessary
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<class UItemAttributeBase>> ItemAttributeTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class UItemAttributeBase*> ItemAttributes;

	FModularItemData() :
		Id(FGuid()), // Empty id, will initialize through item creator
		Icon(nullptr),
		ItemName(TEXT("")),
		DisplayName(TEXT("")),
		Description(TEXT("")),
		MaxStack(1)
	{}

public:

	/**
	 * Init item id if currently uninitilized
	 */
	void InitItemId()
	{ 
		if (!Id.IsValid()) Id = FGuid::NewGuid();
	}

	const FGuid GetItemId() const
	{
		return Id;
	}

	/**
	 * Check if this is an valid item data set
	 * 
	 * If both item name and id is set then it is consider valid
	 */
	bool IsValid() const
	{
		return Id.IsValid()
			&& ItemName != NAME_None;
	}
};

/**
 * This is a reference to the item from the modular item database
 */
USTRUCT(BlueprintType)
struct FModularItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemReferenceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid ItemReferenceId;


	FModularItem() :
		ItemReferenceName(NAME_None),
		ItemReferenceId(FGuid())
	{}
};