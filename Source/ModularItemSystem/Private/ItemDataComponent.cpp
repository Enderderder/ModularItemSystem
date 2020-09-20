// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataComponent.h"

#include "ModularItemSubsystem.h"

// Sets default values for this component's properties
UItemDataComponent::UItemDataComponent()
{
	// TODO: Turn tick off if no need (most likely)
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// Default values
	ItemName = NAME_None;

}

bool UItemDataComponent::GetCurrentItemData(FItemData& _outItemData) const
{
	_outItemData = UModularItemSubsystem::Get()->GetItemByName(ItemName);

	return _outItemData.ItemName == NAME_None ? false : true;
}

// Called when the game starts
void UItemDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UItemDataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#if WITH_EDITOR
void UItemDataComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if the property that changed is ItemName
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UItemDataComponent, ItemName))
	{
		OnItemNameChanged(PropertyChangedEvent);
	}
}

void UItemDataComponent::OnItemNameChanged(FPropertyChangedEvent& PropertyChangedEvent)
{	
	bool bItemExists = GetCurrentItemData(ItemDataPreview);
	if (!bItemExists)
	{
		// It is invalid
	}
}

#endif // WITH_EDITOR
