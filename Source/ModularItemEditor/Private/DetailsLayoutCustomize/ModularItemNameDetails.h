// Copyright Richard Wulansari. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"
#include "ModularItemTypes.h"

class IDetailLayoutBuilder;
class IPropertyHandle;
class SModularItemSelector;
class SItemDetailPreview;


/**
 * 
 */
class FModularItemStructCustomization : public IPropertyTypeCustomization
{
public:
	// Creates an instance of FModularItemNameDetails
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:
	/** IPropertyTypeCustomization interface */
	void CustomizeHeader(TSharedRef<IPropertyHandle> _propertyHandle, FDetailWidgetRow& _headerRow, IPropertyTypeCustomizationUtils& _customizationUtils) override;
	void CustomizeChildren(TSharedRef<IPropertyHandle> _propertyHandle, IDetailChildrenBuilder& _childBuilder, IPropertyTypeCustomizationUtils& _customizationUtils) override;
	/** End IPropertyTypeCustomization interface */

private:

	/** Gets the current item being selected */
	TOptional<FModularItemData> GetCurrentItemData() const;

	/** Updates the property when new item is selected */
	void OnItemChanged(const FModularItemData& _itemData);

	/** Creates the item selector widget */
	TSharedRef<SWidget> CreateItemSelector();

private:

	/** Whether or not we are viewing inner properties only */
	bool									bShowOnlyInnerProperties;

	/** Widget references */
	TSharedPtr<SModularItemSelector>		ItemSelector;
	TSharedPtr<SItemDetailPreview>			ItemDetailPreview;

	/** Handle to the FModularItem struct property */
	TSharedPtr<IPropertyHandle>				PropertyHandle;

	/** Currently selected item data */
	TSharedPtr<TOptional<FModularItemData>>	CurrentItemData;

};