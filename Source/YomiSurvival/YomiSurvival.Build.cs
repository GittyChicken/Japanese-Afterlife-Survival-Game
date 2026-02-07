// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

using UnrealBuildTool;

public class YomiSurvival : ModuleRules
{
	public YomiSurvival(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"ProceduralMeshComponent",
			"Niagara",
			"PhysicsCore",
			"NetCore",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Water"
		});

		// Enable IWYU
		bEnforceIWYU = true;

		// Enable exceptions for error handling
		bEnableExceptions = true;
	}
}
