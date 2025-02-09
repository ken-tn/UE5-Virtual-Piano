// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class Piano : ModuleRules
{
	public Piano(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "FluidSynthUE" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Project directory -> Path.Combine(ModuleDirectory, "../../").
        PublicIncludePaths.AddRange(
		[
			// ... add public include paths required here ...
			Path.Combine(ModuleDirectory, "../../", "Plugins/FluidSynthUE/ThirdParty/bin")
		]
		);

        // FluidSynthUE Plugin DLL dependencies
        // libfluidsynth - 3.dll has ThirdParty/ bin dll dependencies
        // DLLs are packaged into Binaries\Win64
        string DLLDirectory = Path.Combine(ModuleDirectory, "../../", "Plugins/FluidSynthUE/ThirdParty/bin");
		if (!Directory.Exists(DLLDirectory))
		{
			Console.WriteLine("FluidSynth DLL directory not found! Searched at " + DLLDirectory);
		}
		else
		{
			foreach (string DLLName in Directory.GetFiles(DLLDirectory, "*.dll", SearchOption.AllDirectories))
			{
				string runtimeDLLPath = Path.Combine("$(TargetOutputDir)/", Path.GetFileName(DLLName));
                Console.WriteLine("Runtime DLLPath: " + runtimeDLLPath);
                RuntimeDependencies.Add(runtimeDLLPath, DLLName);
			}
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
