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
				Path.Combine(PluginDirectory, "Source/ThirdParty/include")
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
				// Add fluidsynth lib files
				Path.Combine(PluginDirectory, "Source/ThirdParty/lib", "libfluidsynth-3.dll.a"),
				Path.Combine(PluginDirectory, "Source/ThirdParty/lib", "libfluidsynth-3.a")
			]
			);

		string DLLDirectory = Path.Combine(PluginDirectory, "Source/ThirdParty/bin");
        if (!Directory.Exists(DLLDirectory))
        {
            Console.WriteLine("FluidSynth DLL directory not found! Searched at " + DLLDirectory);
        }
        else
        {
			foreach (string DLLPath in Directory.GetFiles(DLLDirectory, "*.dll", SearchOption.AllDirectories))
			{
				// Copy DLLs to same output directory as the executable at build time
                Console.WriteLine("Runtime DLLPath: " + "$(TargetOutputDir)/" + Path.GetFileName(DLLPath));
                RuntimeDependencies.Add("$(TargetOutputDir)/" + Path.GetFileName(DLLPath), DLLPath);
			}
		}

		// note: PublicDelayLoadDLLs accepts only filename, add directory with PublicIncludePaths.Add
		// libfluidsynth-3.dll has ThirdParty/bin dll dependencies
		PublicDelayLoadDLLs.Add("libfluidsynth-3.dll");
	   
	}
}
