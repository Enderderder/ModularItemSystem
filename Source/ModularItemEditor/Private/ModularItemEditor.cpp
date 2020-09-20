// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ModularItemEditor.h"
#include "ModularItemEditorStyle.h"
#include "ModularItemEditorCommands.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

// Widget build block includes
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SItemCreator.h"

static const FName ModularItemSystemTabName("ModularItemSystem");

#define LOCTEXT_NAMESPACE "FModularItemEditorModule"

void FModularItemEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FModularItemEditorStyle::Initialize();
	FModularItemEditorStyle::ReloadTextures();

	FModularItemEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FModularItemEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FModularItemEditorModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> menuExtender = MakeShareable(new FExtender());
		menuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FModularItemEditorModule::AddMenuExtension));

		levelEditorModule.GetMenuExtensibilityManager()->AddExtender(menuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FModularItemEditorModule::AddToolbarExtension));
		
		levelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ModularItemSystemTabName, FOnSpawnTab::CreateRaw(this, &FModularItemEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FModularItemSystemTabTitle", "ModularItemSystem"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FModularItemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FModularItemEditorStyle::Shutdown();

	FModularItemEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ModularItemSystemTabName);
}

TSharedRef<SDockTab> FModularItemEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& _spawnTabArgs) const
{
	auto itemCreator = SNew(SItemCreator);
	itemCreator->Initialize();

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			itemCreator
		];
}

void FModularItemEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ModularItemSystemTabName);
}

void FModularItemEditorModule::AddMenuExtension(FMenuBuilder& _builder)
{
	_builder.AddMenuEntry(FModularItemEditorCommands::Get().OpenPluginWindow);
}

void FModularItemEditorModule::AddToolbarExtension(FToolBarBuilder& _builder)
{
	_builder.AddToolBarButton(FModularItemEditorCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularItemEditorModule, ModularItemEditor)