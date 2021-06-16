// Copyright Epic Games, Inc. All Rights Reserved.

#include "FluidSynthUE4.h"
#include "fluidsynth.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFluidSynthUE4Module"

void FFluidSynthUE4Module::StartupModule()
{
	// Get the base directory of this plugin
	FString BaseDir = "D:/Documents/Unreal Projects/Piano/Plugins/FluidSynthUE4";//IPluginManager::Get().FindPlugin("FluidSynthUE4")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("libfluidsynth-3.dll"));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *LibraryPath);
	DLLHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	UE_LOG(LogTemp, Warning, TEXT("%s"), DLLHandle ? "true" : "false");

	if (DLLHandle)
	{
		// Call the test function in the third party library that opens a message box
		//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Success"));
	}
	else
	{
		//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed loading plugin"));
	}
}

void FFluidSynthUE4Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FPlatformProcess::FreeDllHandle(DLLHandle);
	DLLHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFluidSynthUE4Module, FluidSynthUE4)