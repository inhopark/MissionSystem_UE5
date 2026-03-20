// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MissionSystem : ModuleRules
{
	public MissionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });

        PrivateIncludePaths.AddRange(new string[] { "MissionSystem" });
    }
}
