// Fill out your copyright notice in the Description page of Project Settings.

#include "SItemDetailPreview.h"

#include "Widgets/Layout/SExpandableArea.h"

//#include "ItemAttributeBase.h"

#define LOCTEXT_NAMESPACE "ItemDetailPreview"


void SItemDetailPreview::Refresh(const FModularItemData& _newItemData)
{
	BuildItemBasicDetail(_newItemData);
	BuildItemAttributeDetail(_newItemData);
}

void SItemDetailPreview::BuildItemBasicDetail(const FModularItemData& _itemData)
{
	if (ItemDetailBox.IsValid())
	{
		ItemDetailBox->ClearChildren();
		
		// Pre-build all the details
		ItemIconBrush.Reset();
		ItemIconBrush = MakeShareable(new FSlateBrush);
		ItemIconBrush->SetResourceObject(_itemData.Icon);
		TSharedRef<SBox> itemIconWidget = SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.WidthOverride(FOptionalSize(42.0f))
			.HeightOverride(FOptionalSize(42.0f))
			[
				SNew(SImage)
				.Image(ItemIconBrush.Get())
			];

		TSharedRef<STextBlock> ItemIdWidget = SNew(STextBlock)
			.Text(FText::FromString(_itemData.GetItemId().ToString(EGuidFormats::DigitsWithHyphens)));

		TSharedRef<STextBlock> ItemNameWidget = SNew(STextBlock)
			.Text(FText::FromName(_itemData.ItemName));

		TSharedRef<STextBlock> ItemDisplayNameWidget = SNew(STextBlock)
			.Text(FText::FromString(_itemData.DisplayName));

		TSharedRef<STextBlock> ItemDescriptionWidget = SNew(STextBlock)
			.AutoWrapText(true)
			.Text(FText::FromString(_itemData.Description));

		TSharedRef<STextBlock> IsInfiniteStackWidget = SNew(STextBlock)
			.Text(_itemData.bInifiniteStack ? LOCTEXT("True", "True") : LOCTEXT("False", "False"));

		// Add them into item details
		CreateDetailsRow(LOCTEXT("ItemIcon", "Icon"), itemIconWidget);
		CreateDetailsRow(LOCTEXT("ItemId", "ID"), ItemIdWidget);
		CreateDetailsRow(LOCTEXT("ItemName", "Name"), ItemNameWidget);
		CreateDetailsRow(LOCTEXT("ItemDisplayName", "Display Name"), ItemDisplayNameWidget);
		CreateDetailsRow(LOCTEXT("ItemDescription", "Description"), ItemDescriptionWidget);
		CreateDetailsRow(LOCTEXT("IsInfiniteStack", "Infinite Stack"), IsInfiniteStackWidget);
		if (!_itemData.bInifiniteStack)
		{
			TSharedRef<STextBlock> ItemMaxStackWidget = SNew(STextBlock)
				.Text(FText::AsNumber(_itemData.MaxStack));

			CreateDetailsRow(LOCTEXT("ItemMaxStack", "Max Stack"), ItemMaxStackWidget);
		}
	}
}

void SItemDetailPreview::CreateDetailsRow(const FText& _rowName, TSharedRef<SWidget> _rowContent)
{
	FSlateBorderBrush* borderBrush = new FSlateBorderBrush(NAME_None, FMargin(0.0f, 0.0f, 1.0f, 0.0f));

	ItemDetailBox->AddSlot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.MaxWidth(100.0f)
			[
				SNew(SBox)
				.WidthOverride(100.0f)
				[
					SNew(SBorder)
					.VAlign(VAlign_Center)
					.Padding(FMargin(10.0f, 5.0f))
					.BorderImage(borderBrush)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Right)
						.Text(_rowName)
						//.Text(FText::Join(FText(), TArray<FText>({ _rowName, FText::FromString(": ") })))
					]
				]
				
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			//.AutoWidth()
			.Padding(10.0f, 10.0f)
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				[
					_rowContent
				]
			]
		];
}

void SItemDetailPreview::BuildItemAttributeDetail(const FModularItemData& _itemData)
{
	auto BuildAttributeView = [](UItemAttributeBase* _itemAttribute)
		-> TSharedPtr<IDetailsView>
	{
		// Creates the details view
		FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		FDetailsViewArgs viewArgs;
		viewArgs.bAllowSearch = false;
		viewArgs.bHideSelectionTip = true;
		viewArgs.bShowScrollBar = false;
		viewArgs.bShowActorLabel = false;
		viewArgs.bLockable = false;

		TSharedPtr<IDetailsView> attributeDetailView = propertyModule.CreateDetailView(viewArgs);
		if (attributeDetailView.IsValid())
		{
			attributeDetailView->SetObject(_itemAttribute);
			attributeDetailView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateLambda([]()
				{return false;}));

			return attributeDetailView;
		}

		return nullptr;
	};

	if (ItemAttributeBox.IsValid())
	{
		ItemAttributeBox->ClearChildren();

		for (UItemAttributeBase* itemAttribute : _itemData.ItemAttributes)
		{
			if (itemAttribute)
			{
				FName attributName = itemAttribute->GetFName();

				TSharedPtr<IDetailsView> attributeDetail = BuildAttributeView(itemAttribute);

				ItemAttributeBox->AddSlot()
					[
						SNew(SExpandableArea)
						.InitiallyCollapsed(true)
						.AreaTitle(FText::FromName(attributName))
						.BodyContent()
						[
							attributeDetail.ToSharedRef()
						]
					];
			}
		}
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemDetailPreview::Construct(const FArguments& _inArgs)
{
	CurrentItem = _inArgs._CurrentItem;

	SAssignNew(ItemDetailBox, SVerticalBox);
	SAssignNew(ItemAttributeBox, SVerticalBox);

	FModularItemData itemData;
	TOptional<FModularItemData> otpItemdata = CurrentItem.Get();
	if (otpItemdata.IsSet())
	{
		itemData = otpItemdata.GetValue();
	}

	BuildItemBasicDetail(itemData);
	BuildItemAttributeDetail(itemData);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				ItemDetailBox.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			ItemAttributeBox.ToSharedRef()
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE