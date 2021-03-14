// Copyright Richard Wulansari. All Rights Reserved.

#include "ModularItemNameDetails.h"

#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SModularItemSelector.h"
#include "Widgets/SItemDetailPreview.h"

#include "ModularItemSubsystem.h"

#define LOCTEXT_NAMESPACE "ModularItemStructCustomization"

/************************************************************************/
/* FModularItemStructCustomization                                      */
/************************************************************************/
TSharedRef<IPropertyTypeCustomization> FModularItemStructCustomization::MakeInstance()
{
	return MakeShareable(new FModularItemStructCustomization);
}

void FModularItemStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> _propertyHandle, FDetailWidgetRow& _headerRow, IPropertyTypeCustomizationUtils& _customizationUtils)
{
	PropertyHandle = _propertyHandle;

	bShowOnlyInnerProperties = _propertyHandle->GetProperty()->HasMetaData(TEXT("ShowOnlyInnerProperties"));
	
	if (!bShowOnlyInnerProperties)
	{
		_headerRow
		.NameContent()
		[
			_propertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(1000.0f)
		[
			CreateItemSelector()
		];
	}
}

void FModularItemStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> _propertyHandle, IDetailChildrenBuilder& _childBuilder, IPropertyTypeCustomizationUtils& _customizationUtils)
{	
	// If we only shows inner property, adds the item search combox in here instead
	if (bShowOnlyInnerProperties)
	{
		// Build the item selector
		_childBuilder.AddCustomRow(LOCTEXT("ItemSelector", "Item Selector"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ItemSelector", "Item Selector"))
			]
			.ValueContent()
			.MinDesiredWidth(1000.f)
			[
				CreateItemSelector()
			];
	}

	// Build the detail preview
	_childBuilder.AddCustomRow(LOCTEXT("DetailPreview", "Detail Preview"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ItemPreview", "Item Preview"))
		]
		.ValueContent()
		.MinDesiredWidth(1000.f)
		[
			SAssignNew(ItemDetailPreview, SItemDetailPreview)
			.CurrentItem(this, &FModularItemStructCustomization::GetCurrentItemData)
		];
}

TOptional<FModularItemData> FModularItemStructCustomization::GetCurrentItemData() const
{
	// If multiple instance has been selected, we will skip it by passing an empty optional data
	TArray<void*> rawData;
	PropertyHandle->AccessRawData(rawData);

	if (rawData.Num() != 1 ||
		rawData[0] == nullptr)
	{
		return TOptional<FModularItemData>();
	}

	UModularItemSubsystem* miSys = UModularItemSubsystem::Get();

	const FModularItem itemRef = *(FModularItem*)rawData[0];

	FName itemRefName = itemRef.ItemReferenceName;
	if (itemRefName != NAME_None)
	{
		FModularItemData itemData = miSys->GetItemByName(itemRefName);
		if (itemData.IsValid())
		{
			return itemData;
		}
		else
		{
			// When we cannot find the item through name, maybe the name of the item has been changed
			// We can try again using ID, this will be slower as id is not the map key of the item data sheet
			// We will need to rename the item reference name if we successfully find the item
			FGuid itemRefId = itemRef.ItemReferenceId;
			if (itemRefId.IsValid())
			{
				itemData = miSys->GetItemById(itemRefId);

				TSharedRef<IPropertyHandle> itemNameHandle = PropertyHandle->GetChildHandle("ItemReferenceName").ToSharedRef();
				itemNameHandle->SetValue(itemData.ItemName);
				
				return itemData;
			}
		}
	}

	return FModularItemData();
}

void FModularItemStructCustomization::OnItemChanged(const FModularItemData& _itemData)
{
	// Writes the value
	{
		FScopedTransaction transaction(FText::Format(LOCTEXT("EditModularItemPropertyTransaction", "Edit {0}"), PropertyHandle->GetPropertyDisplayName()));

		// We will handle the transactions as a whole instead of individual one
		EPropertyValueSetFlags::Type ModularItemFlag = EPropertyValueSetFlags::NotTransactable;

		// Item name property writing
		FName itemName = _itemData.ItemName;
		TSharedRef<IPropertyHandle> itemNameHandle = PropertyHandle->GetChildHandle("ItemReferenceName").ToSharedRef();
		itemNameHandle->SetValue(itemName, ModularItemFlag | EPropertyValueSetFlags::InteractiveChange);

		// Item id property writing
		FGuid itemId = _itemData.GetItemId();
		TSharedRef<IPropertyHandle> itemIdHandle = PropertyHandle->GetChildHandle("ItemReferenceId").ToSharedRef();
		for (uint32 childIndex = 0; childIndex < 4; ++childIndex)
		{
			TSharedRef<IPropertyHandle> idChildHandle = itemIdHandle->GetChildHandle(childIndex).ToSharedRef();
			idChildHandle->SetValue((int32)itemId[childIndex],
				childIndex != 3 ? ModularItemFlag | EPropertyValueSetFlags::InteractiveChange : ModularItemFlag);
		}
	}
	
	// Refresh the preview display
	if (ItemDetailPreview.IsValid())
	{
		ItemDetailPreview->Refresh(_itemData);
	}
}

TSharedRef<SWidget> FModularItemStructCustomization::CreateItemSelector()
{
	ItemSelector.Reset();
	ItemSelector = SNew(SModularItemSelector)
		.CurrentItem(this, &FModularItemStructCustomization::GetCurrentItemData)
		.OnItemChanged(this, &FModularItemStructCustomization::OnItemChanged);

	ItemSelector->SetEnabled(PropertyHandle->IsEditable());

	return ItemSelector.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE