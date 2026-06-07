// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BachelorsThesis : ModuleRules
{
	public BachelorsThesis(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"BachelorsThesis",
			"BachelorsThesis/Variant_Horror",
			"BachelorsThesis/Variant_Horror/UI",
			"BachelorsThesis/Variant_Shooter",
			"BachelorsThesis/Variant_Shooter/AI",
			"BachelorsThesis/Variant_Shooter/UI",
			"BachelorsThesis/Variant_Shooter/Weapons",
			"BachelorsThesis/DataModel",
			"BachelorsThesis/Framework",
			"BachelorsThesis/PlayerServices",
			"BachelorsThesis/ReactionHelpers",
			"BachelorsThesis/Highlight",
			"BachelorsThesis/Player",
			"BachelorsThesis/DemoActors",
			"BachelorsThesis/Tests"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
