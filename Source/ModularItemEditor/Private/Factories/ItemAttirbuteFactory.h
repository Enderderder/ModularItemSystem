// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ItemAttirbuteFactory.generated.h"

/**
 * 
 */
UCLASS()
class UItemAttirbuteFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UItemAttirbuteFactory();

public:

	// ~ Begin UFactory Interface
	UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// ~ End UFactory Interface
};
