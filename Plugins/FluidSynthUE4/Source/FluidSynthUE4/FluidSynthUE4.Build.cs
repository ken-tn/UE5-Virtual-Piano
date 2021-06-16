// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class FluidSynthUE4 : ModuleRules
{
	public FluidSynthUE4(ReadOnlyTargetRules Target) : base(Target)
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
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		// Add any macros that need to be set
		//PublicDefinitions.Add("WITH_MYTHIRDPARTYLIBRARY=1");

		// Add any import libraries or static libraries
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "fluidsynth.lib"));

		// Delay-load the DLL, so we can load it from the right place first
		PublicDelayLoadDLLs.Add("libfluidsynth-3.dll");

		// Add any include paths for the plugin
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

		// Ensure that the DLL is staged along with the executable
		RuntimeDependencies.Add("$(PluginDir)/libfluidsynth-3.dll");
	}
}
