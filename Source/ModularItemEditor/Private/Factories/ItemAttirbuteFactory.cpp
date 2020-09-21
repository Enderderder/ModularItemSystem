// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemAttirbuteFactory.h"

#include "ItemAttributeBase.h"

UItemAttirbuteFactory::UItemAttirbuteFactory()
{
	Formats.Add(FString(TEXT("txt;")) + NSLOCTEXT("UTextAssetFactory", "FormatTxt", "Text File").ToString());

	SupportedClass = UItemAttributeBase::StaticClass();
	bCreateNew = false;
	bEditorImport = false;
}

UObject* UItemAttirbuteFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UItemAttributeBase* itemAttribute = nullptr;
	itemAttribute = NewObject<UItemAttributeBase>(InParent, InClass, InName, Flags);	
	
	bOutOperationCanceled = false;

	return itemAttribute;
}

UObject* UItemAttirbuteFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UItemAttributeBase* itemAttribute = nullptr;
	itemAttribute = NewObject<UItemAttributeBase>(InParent, InClass, InName, Flags);

	return itemAttribute;
}
