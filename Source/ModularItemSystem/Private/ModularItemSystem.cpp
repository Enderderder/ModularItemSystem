// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ModularItemSystem.h"

static const FName ModularItemSystemTabName("ModularItemSystem");

#define LOCTEXT_NAMESPACE "ModularItemSystemModule"

void FModularItemSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FModularItemSystemModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularItemSystemModule, ModularItemSystem)