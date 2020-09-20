// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ModularItemEditorStyle.h"

class FModularItemEditorCommands : public TCommands<FModularItemEditorCommands>
{
public:

	FModularItemEditorCommands()
		: TCommands<FModularItemEditorCommands>(TEXT("ModularItemEditor"), NSLOCTEXT("Contexts", "ModularItemEditor", "ModularItemEditor Plugin"), NAME_None, FModularItemEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};