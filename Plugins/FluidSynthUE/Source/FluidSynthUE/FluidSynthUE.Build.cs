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
			[
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "ThirdParty/include")
			]
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

		PublicAdditionalLibraries.AddRange(
			[
				Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth-3.dll.a"),
				Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth-3.a")
			]
			);

		string DLLDirectory = Path.Combine(PluginDirectory, "ThirdParty/bin");
		foreach (string DLLName in Directory.GetFiles(DLLDirectory, "*.dll", SearchOption.AllDirectories))
		{
            // Copy DLLs to same output directory as the executable at build time
            string runtimeDLLPath = Path.Combine("$(TargetOutputDir)/", Path.GetFileName(DLLName));
			RuntimeDependencies.Add(runtimeDLLPath, DLLName);
		}

		// note: PublicDelayLoadDLLs accepts only filename, add directory with PublicIncludePaths.Add
		// libfluidsynth-3.dll has ThirdParty/bin dll dependencies
		PublicDelayLoadDLLs.Add("libfluidsynth-3.dll");
	   
	}
}
