// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ModularItemTypes.h"
#include "ItemDataComponent.generated.h"

/**
 * An actor component that can hold item data
 */
UCLASS( ClassGroup=(ModularItemSystem), meta=(BlueprintSpawnableComponent) )
class MODULARITEMSYSTEM_API UItemDataComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/** The name of the item this component is representing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FName ItemName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	FModularItemData ItemDataPreview;
#endif // WITH_EDITORONLY_DATA

public:	
	UItemDataComponent();

public:

	/**
	 * Gets the item data from the subsystem with current item name
	 * @param _outItemData The output struct data (Will be an empty struct if the item do not exist)
	 * @return True if the item actually exists
	 */
	UFUNCTION(BlueprintPure, Category = "ModularItemComponent")
	UPARAM(DisplayName = "ItemExist?") bool GetCurrentItemData(FModularItemData& _outItemData) const;

protected:
	// ~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	// ~ End UActorComponent Interface

public:	
	// ~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	// ~ End UActorComponent Interface

#if WITH_EDITOR
private:
	/** Called when the item data name variable changed so we can handle the display item info */
	void OnItemNameChanged(FPropertyChangedEvent& PropertyChangedEvent);
#endif // WITH_EDITOR
};
