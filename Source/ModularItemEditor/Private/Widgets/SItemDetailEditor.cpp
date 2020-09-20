// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemDetailEditor.h"
#include "SlateOptMacros.h"

#include "ModularItemSubsystem.h"
#include "ItemAttributeBase.h"

#include "PropertyEditorModule.h"
#include "IStructureDetailsView.h"
#include "IDetailsView.h"


#define LOCTEXT_NAMESPACE "SItemDetailEditor"

class FStructFromItemDataTable : public FStructOnScope
{
	TWeakObjectPtr<UDataTable> DataTable;
	FName RowName;

public:
	FStructFromItemDataTable(UDataTable* InDataTable, FName _inRowName)
		: FStructOnScope()
		, DataTable(InDataTable)
		, RowName(_inRowName)
	{}

	virtual uint8* GetStructMemory() override
	{
		return (DataTable.IsValid() && !RowName.IsNone()) ? DataTable->FindRowUnchecked(RowName) : nullptr;
	}

	virtual const uint8* GetStructMemory() const override
	{
		return (DataTable.IsValid() && !RowName.IsNone()) ? DataTable->FindRowUnchecked(RowName) : nullptr;
	}

	virtual const UScriptStruct* GetStruct() const override
	{
		return DataTable.IsValid() ? DataTable->GetRowStruct() : nullptr;
	}

	virtual UPackage* GetPackage() const override
	{
		return DataTable.IsValid() ? DataTable->GetOutermost() : nullptr;
	}

	virtual void SetPackage(UPackage* InPackage) override
	{
	}

	virtual bool IsValid() const override
	{
		return !RowName.IsNone()
			&& DataTable.IsValid()
			&& DataTable->GetRowStruct()
			&& DataTable->FindRowUnchecked(RowName);
	}

	virtual void Destroy() override
	{
		DataTable = nullptr;
		RowName = NAME_None;
	}

	FName GetRowName() const
	{
		return RowName;
	}
};

SItemDetailEditor::SItemDetailEditor() :
	SCompoundWidget()
{}

SItemDetailEditor::~SItemDetailEditor()
{}

void SItemDetailEditor::NotifyPreChange(FProperty* _propertyAboutToChange)
{
	check(ItemDataTable.IsValid());
	ItemDataTable->Modify();

	FDataTableEditorUtils::BroadcastPreChange(ItemDataTable.Get(), FDataTableEditorUtils::EDataTableChangeInfo::RowData);
}

void SItemDetailEditor::NotifyPostChange(const FPropertyChangedEvent& _propertyChangedEvent, FProperty* _propertyThatChanged)
{
	check(ItemDataTable.IsValid());

	FDataTableEditorUtils::BroadcastPostChange(ItemDataTable.Get(), FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	ItemDataTable->MarkPackageDirty();
}

void SItemDetailEditor::PreChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info)
{
	if (Struct && (GetScriptStruct() == Struct))
	{
		CleanBeforeChange();
	}
}

void SItemDetailEditor::PostChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info)
{
	if (Struct && (GetScriptStruct() == Struct))
	{
		RefreshNameList();
		Restore();
	}
}

void SItemDetailEditor::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if ((Changed == ItemDataTable.Get()) && (FDataTableEditorUtils::EDataTableChangeInfo::RowList == Info))
	{
		CleanBeforeChange();
	}
}

void SItemDetailEditor::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if ((Changed == ItemDataTable.Get()) && (FDataTableEditorUtils::EDataTableChangeInfo::RowList == Info))
	{
		RefreshNameList();
		Restore();
	}
}

void SItemDetailEditor::SelectItem(FName _itemName)
{
	TSharedPtr<FName> newSelectedName;
	for (auto name : CachedRowNames)
	{
		if (name.IsValid() && (*name == _itemName))
		{
			newSelectedName = name;
		}
	}
	if (!newSelectedName.IsValid())
	{
		newSelectedName = MakeShareable(new FName(_itemName));
	}
	OnSelectionChanged(newSelectedName, ESelectInfo::Direct);
}

void SItemDetailEditor::OnPropertyChangeFinished(const FPropertyChangedEvent& _propertyChangedEvent)
{
	FProperty* propertyChanged = _propertyChangedEvent.Property;

	if (!propertyChanged) return;

	// Do revert if the new item name is invalid or repeated
	if (IsChangingItemName(propertyChanged))
	{
		FName newItemName = GetFNameValueFromProperty(propertyChanged, CurrentRow->GetStructMemory());
		FName dataTableRowName = ((FStructFromItemDataTable*)CurrentRow.Get())->GetRowName();

		if (newItemName != dataTableRowName)
		{
			bool bNeedToRevertChange = false;
			if (newItemName == NAME_None || !FName::IsValidXName(newItemName.ToString(), INVALID_NAME_CHARACTERS))
			{
				bNeedToRevertChange = true;

				// Popup an error dialog here
				const FText Message = FText::Format(LOCTEXT(
					"InvalidItemName",
					"'{0}' is not a valid item name. Note: Item name will sync with the data table row name (Data table raw map ID)"),
					FText::FromName(newItemName));
				FMessageDialog::Open(EAppMsgType::Ok, Message);
			}
			else
			{
				for (auto name : CachedRowNames)
				{
					if (name.IsValid() && name->IsValid() && (*name == newItemName))
					{
						bNeedToRevertChange = true;

						// The name already exists
						// Popup an error dialog here
						const FText Message = FText::Format(LOCTEXT(
							"DuplicateRowName",
							"'{0}' is already used as a row name in this table. Note: Item name will sync with the data table row name (Data table raw map ID)"),
							FText::FromName(newItemName));
						FMessageDialog::Open(EAppMsgType::Ok, Message);
						break;
					}
				}
			}

			// Revert the change if needed
			if (bNeedToRevertChange)
			{
				FNameProperty* itemNameProperty = CastField<FNameProperty>(propertyChanged);
				itemNameProperty->SetPropertyValue_InContainer(CurrentRow->GetStructMemory(), dataTableRowName);
			}
			else
			{
				UDataTable* itemDataTable = ItemDataTable.Get();

				const FName oldName = GetCurrentName();
				FDataTableEditorUtils::RenameRow(itemDataTable, oldName, newItemName);
				FDataTableEditorUtils::SelectRow(itemDataTable, newItemName);

				*SelectedName = newItemName;
			}
		}
	}

	// Refresh the attribute detail views if the array has changed
	FName changedPropertyName = propertyChanged->GetFName();
	if (changedPropertyName == TEXT("ItemAttributes"))
	{
		CleanAttributeDetailViews();
		RebuildAttributeDetailViews();
	}

	// HACK: Test code for changing the name of the data asset
// 	if (changedPropertyName == TEXT("ItemAttributes"))
// 	{
// 		if (propertyChanged->IsA(FArrayProperty::StaticClass()))
// 		{
// 			FArrayProperty* attributeArrayProperty = CastField<FArrayProperty>(propertyChanged);
// 			if (attributeArrayProperty)
// 			{
// 				TArray<UItemAttributeBase*> attrArray =
// 					*attributeArrayProperty->ContainerPtrToValuePtr<TArray<UItemAttributeBase*>>(CurrentRow->GetStructMemory());
// 				for (auto attribute : attrArray)
// 				{
// 					if (attribute)
// 					{
// 
// 					}
// 				}
// 
// 				UE_LOG(LogModularItemSystem, Warning, TEXT("Attribute array changed"));
// 			}
// 		}
// 	}
}

bool SItemDetailEditor::IsChangingItemName(FProperty* _changingProperty) const
{
	FName changedPropertyName = _changingProperty->GetFName();
	return (changedPropertyName == TEXT("ItemName") && _changingProperty->IsA(FNameProperty::StaticClass()));
}

FName SItemDetailEditor::GetFNameValueFromProperty(FProperty* _changingProperty, const void *_a) const
{
	FNameProperty* itemNameProperty = CastField<FNameProperty>(_changingProperty);

	return (itemNameProperty)
		? *itemNameProperty->ContainerPtrToValuePtr<FName>(_a)
		: NAME_None;
}

void SItemDetailEditor::RefreshNameList()
{
	CachedRowNames.Empty();
	if (ItemDataTable.IsValid())
	{
		auto rowNames = ItemDataTable->GetRowNames();
		for (auto rowName : rowNames)
		{
			CachedRowNames.Add(MakeShareable(new FName(rowName)));
		}
	}
}

void SItemDetailEditor::CleanBeforeChange()
{
	CleanAttributeDetailViews();

	if (StructureDetailsView.IsValid())
	{
		StructureDetailsView->SetStructureData(NULL);
	}
	if (CurrentRow.IsValid())
	{
		CurrentRow->Destroy();
		CurrentRow.Reset();
	}
}

void SItemDetailEditor::Restore()
{
	if (!SelectedName.IsValid() || !SelectedName->IsNone())
	{
		if (SelectedName.IsValid())
		{
			auto currentName = *SelectedName;
			SelectedName = NULL;
			for (auto element : CachedRowNames)
			{
				if (*element == currentName)
				{
					SelectedName = element;
					break;
				}
			}
		}

		if (!SelectedName.IsValid() && CachedRowNames.Num() && CachedRowNames[0].IsValid())
		{
			SelectedName = CachedRowNames[0];
		}
	}

	auto finalName = SelectedName.IsValid() ? *SelectedName : NAME_None;
	CurrentRow = MakeShareable(new FStructFromItemDataTable(ItemDataTable.Get(), finalName));
	if (StructureDetailsView.IsValid())
	{
		StructureDetailsView->SetCustomName(FText::FromName(finalName));
		StructureDetailsView->SetStructureData(CurrentRow);

		RebuildAttributeDetailViews();
	}
}

void SItemDetailEditor::CleanAttributeDetailViews()
{
	if (AttributeDetailViewsHolder.IsValid())
	{
		AttributeDetailViewsHolder->ClearChildren();
	}
}

void SItemDetailEditor::RebuildAttributeDetailViews()
{
	// Builds the detail views of all the attribute this item has
	FItemData* itemData = (FItemData*)CurrentRow->GetStructMemory();
	if (itemData && AttributeDetailViewsHolder.IsValid())
	{
		for (UItemAttributeBase* itemAttribute : itemData->ItemAttributes)
		{
			if (itemAttribute)
			{
				// Creates the details view
				FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
				FDetailsViewArgs viewArgs;
				viewArgs.bAllowSearch = false;
				viewArgs.bHideSelectionTip = true;
				viewArgs.bShowScrollBar = false;
				viewArgs.bShowActorLabel = false;
				viewArgs.NotifyHook = this;

				TSharedPtr<IDetailsView> attributeDetailView = propertyModule.CreateDetailView(viewArgs);
				if (attributeDetailView.IsValid())
				{
					// Add a spacer between
					AttributeDetailViewsHolder->AddSlot()[
						SNew(SSpacer).Size(FVector2D(20.0f, 20.0f))
					];

					// Display the name of the item attribute
					FName attributeName = itemAttribute->GetFName();
					AttributeDetailViewsHolder->AddSlot().AutoHeight()[
						SNew(STextBlock).Text(FText::FromName(attributeName))
					];

					// Display the detail of the item attribute
					attributeDetailView->SetObject(itemAttribute);
					AttributeDetailViewsHolder->AddSlot().AutoHeight()[
						attributeDetailView.ToSharedRef()
					];
				}
			}
		}
	}
}

UScriptStruct* SItemDetailEditor::GetScriptStruct() const
{
	return ItemDataTable.IsValid() ? ItemDataTable->RowStruct : nullptr;
}

FName SItemDetailEditor::GetCurrentName() const
{
	return SelectedName.IsValid() ? *SelectedName : NAME_None;
}

void SItemDetailEditor::OnSelectionChanged(TSharedPtr<FName> _inItem, ESelectInfo::Type _inSeletionInfo)
{
	if (_inItem.IsValid() && _inItem != SelectedName)
	{
		CleanBeforeChange();

		SelectedName = _inItem;

		Restore();
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemDetailEditor::Construct(const FArguments& _inArgs, UDataTable* _inDataTable)
{
	ItemDataTable = _inDataTable;
	{
		FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		FDetailsViewArgs viewArgs;
		viewArgs.bAllowSearch = false;
		viewArgs.bHideSelectionTip = false;
		viewArgs.bShowScrollBar = false;
		viewArgs.bShowActorLabel = false;
		viewArgs.NotifyHook = this;

		FStructureDetailsViewArgs structureViewArgs;
		structureViewArgs.bShowObjects = false;
		structureViewArgs.bShowAssets = true;
		structureViewArgs.bShowClasses = true;
		structureViewArgs.bShowInterfaces = false;

		StructureDetailsView = propertyModule.CreateStructureDetailView(viewArgs, structureViewArgs, CurrentRow, LOCTEXT("RowValue", "Row Value"));
		StructureDetailsView->GetOnFinishedChangingPropertiesDelegate().AddSP(this, &SItemDetailEditor::OnPropertyChangeFinished);
	}
	{
		AttributeDetailViewsHolder = SNew(SVerticalBox);
	}

	RefreshNameList();
	Restore();

	ChildSlot
	[
		SAssignNew(DetailViewHolder, SVerticalBox)
		+ SVerticalBox::Slot() .AutoHeight()
		[
			StructureDetailsView->GetWidget().ToSharedRef()
		]
		+ SVerticalBox::Slot() .AutoHeight()
		[
			AttributeDetailViewsHolder.ToSharedRef()
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef  LOCTEXT_NAMESPACE