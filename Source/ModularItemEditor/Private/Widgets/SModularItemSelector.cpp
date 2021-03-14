// Copyright Richard Wulansari. All Rights Reserved.

#include "SModularItemSelector.h"

#include "PropertyCustomizationHelpers.h"

#include "SItemDataCompactRowView.h"
#include "SListViewSelectorDropdownMenu.h"

#define LOCTEXT_NAMESPACE "ModularItemSelector"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SModularItemSelector::Construct(const FArguments& _inArgs)
{
	SearchText = FText::GetEmpty();

	CurrentItem			= _inArgs._CurrentItem;
	OnItemChanged		= _inArgs._OnItemChanged;

	ListViewWidth		= _inArgs._ListViewWidth;
	ListViewHeight		= _inArgs._ListViewHeight;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SAssignNew(ItemSelectComboButton, SComboButton)
			//.ButtonStyle(FCoreStyle::Get(), "NoBorder")
			.OnGetMenuContent(this, &SModularItemSelector::GetItemMenuContent)
			.ContentPadding(0)
			.ButtonContent()
			[
				SAssignNew(ComboButtonContent, SBox)
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			PropertyCustomizationHelpers::MakeClearButton(
				FSimpleDelegate::CreateSP(this, &SModularItemSelector::OnSelectionCleared),
				LOCTEXT("ClearItem", "Clear the item selection"))
		]
	];

	// Rebuild the content of the combo button
	RebuildComboButtonContent();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SModularItemSelector::SelectItem(const FModularItemData& _itemData)
{
	CurrentItem.Set(_itemData);

	RebuildComboButtonContent();
	
	OnItemChanged.ExecuteIfBound(_itemData);
}

void SModularItemSelector::RebuildComboButtonContent()
{
	if (!ItemSelectComboButton.IsValid())
		return;

	TOptional<FModularItemData> CurrentItemValue = CurrentItem.Get();
	if (CurrentItemValue.IsSet())
	{
		if (ItemCompactView.IsValid())
		{
			ItemCompactView->Refresh(CurrentItemValue.GetValue());
		}
		else
		{
			SAssignNew(ItemCompactView, SItemDataCompactRowView, CurrentItemValue.GetValue())
				.bDrawBackground(true);
		}

		ComboButtonContent->SetContent(ItemCompactView.ToSharedRef());
	}
	else
	{
		if (!MultiItemWarningTextBlock.IsValid())
		{
			SAssignNew(MultiItemWarningTextBlock, STextBlock)
				.Text(LOCTEXT("MultipleValue", "Multiple Value"));
		}

		ComboButtonContent->SetContent(MultiItemWarningTextBlock.ToSharedRef());
	}
}

FModularItemData SModularItemSelector::GetCurrentItemData() const
{
	TOptional<FModularItemData> CurrentItemValue = CurrentItem.Get();
	if (CurrentItemValue.IsSet())
	{
		return CurrentItemValue.GetValue();
	}

	return FModularItemData();
}

TSharedRef<SWidget> SModularItemSelector::GetItemMenuContent()
{
	if (!MenuContent.IsValid())
	{
		SAssignNew(ItemListView, SModularItemListView)
			.ListItemsSource(&VisibleItems)
			.SelectionMode(ESelectionMode::Single)
			.ConsumeMouseWheel(EConsumeMouseWheel::Always)
			.OnGenerateRow(this, &SModularItemSelector::OnGenerateItemRow)
			.OnSelectionChanged(this, &SModularItemSelector::OnItemSelectionChanged);

		SAssignNew(SearchFilterBox, SSearchBox)
			.OnTextChanged(this, &SModularItemSelector::OnFilterTextChanged);

		MenuContent = SNew(SListViewSelectorDropdownMenu<FModularItemListViewPtr>, SearchFilterBox, ItemListView)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f, 4.f, 4.f, 4.f)
				[
					SearchFilterBox.ToSharedRef()
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f, 4.f, 4.f, 4.f)
				[
					SNew(SBox) 
					.WidthOverride(ListViewWidth)
					.HeightOverride(ListViewHeight)
					[
						ItemListView.ToSharedRef()
					]
				]
			];

		ItemSelectComboButton->SetMenuContentWidgetToFocus(SearchFilterBox);
	}
	else
	{
		ItemListView->SetSelection(nullptr, ESelectInfo::OnNavigation);
	}
	
	RefreshCachedItemList();

	return MenuContent.ToSharedRef();
}

TSharedRef<ITableRow> SModularItemSelector::OnGenerateItemRow(FModularItemListViewPtr _itemPtr, const TSharedRef<STableViewBase>& _ownerList)
{
	return SNew(SItemDataListViewRow, _ownerList)
		.ModularItemPtr(_itemPtr)
		.bUseSmallIcon(true);
}

void SModularItemSelector::OnItemSelectionChanged(FModularItemListViewPtr _itemPtr, ESelectInfo::Type _selectInfo)
{
	// When the user is navigating, do not act upon the selection change
	if (_selectInfo == ESelectInfo::OnNavigation)
		return;

	if (_itemPtr.IsValid())
	{
		ItemSelectComboButton->SetIsOpen(false);

		SelectItem(_itemPtr->ItemData);
	}
}

void SModularItemSelector::OnFilterTextChanged(const FText& _newText)
{
	SearchText = _newText;
	UpdateVisibleItems();
}

void SModularItemSelector::OnSelectionCleared()
{
	SelectItem(FModularItemData());
}

void SModularItemSelector::RefreshCachedItemList()
{
	FModularItemEditorUtils::CacheItemDataTable(AvaliableItems);

	UpdateVisibleItems();
}

void SModularItemSelector::UpdateVisibleItems()
{
	FModularItemEditorUtils::ModularItemFilterPass(SearchText, AvaliableItems, VisibleItems);

	ItemListView->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE