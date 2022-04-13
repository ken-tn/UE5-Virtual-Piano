// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class FluidSynthUE : ModuleRules
{
	public FluidSynthUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		// Add any import libraries or static libraries
		PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth.dll.a"));
		PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth.a"));

		// Add any include paths for the plugin
		PublicIncludePaths.Add(Path.Combine(PluginDirectory, "ThirdParty/include"));
	}
}
