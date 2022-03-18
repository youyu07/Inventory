// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventoryWidget : ModuleRules
{
	public InventoryWidget(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealAny",
				"InventorySystem",
				"UMG"
			}
			);
		
	}
}
