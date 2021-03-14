// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ModularItemEditorCommands.h"

#define LOCTEXT_NAMESPACE "ModularItemEditorModule"

void FModularItemEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ModularItemSystem", "Bring up ModularItemSystem window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
