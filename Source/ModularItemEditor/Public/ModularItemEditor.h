// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

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
};
