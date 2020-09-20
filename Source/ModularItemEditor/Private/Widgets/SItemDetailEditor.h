// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "Misc/NotifyHook.h"
#include "Kismet2/StructureEditorUtils.h"
#include "DataTableEditorUtils.h"

/**
 * 
 */
class SItemDetailEditor : public SCompoundWidget
	, public FNotifyHook
	, public FStructureEditorUtils::INotifyOnStructChanged
	, public FDataTableEditorUtils::INotifyOnDataTableChanged
{
public:
	SLATE_BEGIN_ARGS(SItemDetailEditor){}
	SLATE_END_ARGS()

	SItemDetailEditor();
	virtual ~SItemDetailEditor();

	// FNotifyHook
	virtual void NotifyPreChange(FProperty* PropertyAboutToChange) override;
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	// INotifyOnStructChanged
	virtual void PreChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;
	virtual void PostChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;

	// INotifyOnDataTableChanged
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;

	/**
	 * Select the item within the data table to edit
	 * @param _itemName The name of the item (Row name)
	 */
	void SelectItem(FName _itemName);

private:

	bool IsChangingItemName(FProperty* _changingProperty) const;
	FName GetFNameValueFromProperty(FProperty* _changingProperty, const void *_a) const;

protected:

	TArray<TSharedPtr<FName>> CachedRowNames;
	TSharedPtr<FStructOnScope> CurrentRow;
	FName CachedEditingItemName;
	TSoftObjectPtr<UDataTable> ItemDataTable;
	TSharedPtr<class IStructureDetailsView> StructureDetailsView;
	TSharedPtr<FName> SelectedName;

	void RefreshNameList();
	void CleanBeforeChange();
	void Restore();
	void CleanAttributeDetailViews();
	void RebuildAttributeDetailViews();

	UScriptStruct* GetScriptStruct() const;
	FName GetCurrentName() const;

	virtual void OnSelectionChanged(TSharedPtr<FName> _inItem, ESelectInfo::Type _inSeletionInfo);
	void OnPropertyChangeFinished(const FPropertyChangedEvent& _propertyChangedEvent);

private:

	/** Vertical box that holds all the detail views */
	TSharedPtr<SVerticalBox> DetailViewHolder;

	/** Vertical box that holds all the item attribute details */
	TSharedPtr<SVerticalBox> AttributeDetailViewsHolder;

public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& _inArgs, UDataTable* _inDataTable);

};
