// Copyright Richard Wulansari. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FModularItemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void AddToolbarExtension(FToolBarBuilder& _builder);
	void AddMenuExtension(FMenuBuilder& _builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& _spawnTabArgs) const;

private:
	TSharedPtr<class FUICommandList> PluginCommands;


private:
	void RegisterObjectCustomizations();

	/**
	 * Registers a custom class to the property editor module
	 *
	 * @param ClassName				The class name to register for property customization
	 * @param DetailLayoutDelegate	The delegate to call to get the custom detail layout instance
	 */
	void RegisterCustomClassLayout(FName _className, FOnGetDetailCustomizationInstance _detailLayoutDelegate);

	/**
	* Registers a custom struct
	*
	* @param StructName				The name of the struct to register for property customization
	* @param StructLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomPropertyTypeLayout(FName _propertyTypeName, FOnGetPropertyTypeCustomizationInstance _propertyTypeLayoutDelegate);

private:
	/** List of registered class that we must unregister when the module shuts down */
	TSet<FName> RegisteredClassNames;
	TSet<FName> RegisteredPropertyTypes;
};
