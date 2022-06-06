// Copyright Epic Games, Inc. All Rights Reserved.

#include "FluidSynthUE.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "Windows/PreWindowsApi.h"
#include "windows.h"
#include "Windows/PostWindowsApi.h"

#define LOCTEXT_NAMESPACE "FFluidSynthUEModule"

void FFluidSynthUEModule::StartupModule()
{
#if PLATFORM_WINDOWS
	UE_LOG(LogTemp, Display, TEXT("Loaded"));
#endif
}

void FFluidSynthUEModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFluidSynthUEModule, FluidSynth)
