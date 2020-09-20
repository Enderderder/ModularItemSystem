// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemAttributeBase.generated.h"

/**
 * 
 */
UCLASS(abstract, Blueprintable)
class MODULARITEMSYSTEM_API UItemAttributeBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Attribute")
	TArray<FName> AttributeTags;
};
