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
				Path.Combine(PluginDirectory, "ThirdParty/include")
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

		PublicAdditionalLibraries.AddRange(
			new string[]
			{
				Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth.dll.a"),
				Path.Combine(PluginDirectory, "ThirdParty/lib", "libfluidsynth.a")
			}
			);

		PublicDelayLoadDLLs.Add("libfluidsynth-3.dll");

		string dllFolder = Path.Combine(PluginDirectory, "ThirdParty/dll");
		foreach (string dll in Directory.GetFiles(dllFolder, "*.dll", SearchOption.AllDirectories))
        {
			string dllPath = Path.Combine("$(BinaryOutputDir)/", Path.GetFileName(dll));
			RuntimeDependencies.Add(dllPath, dll);
		}
	}
}
