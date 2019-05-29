// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class OculusQuestTemplate : ModuleRules
{
	public OculusQuestTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Standard Engine Dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		// VR Dependencies
		PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "OculusAvatar" });
	}
}