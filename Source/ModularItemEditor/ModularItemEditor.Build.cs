// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ModularItemEditor : ModuleRules
{
	public ModularItemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        if (Target.Type != TargetType.Editor)
        {
            throw new BuildException("Unable to instantiate ModularItemEditor module for non-editor targets.");
        }


        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"ModularItemSystem",
			}
			);


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"PropertyEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
