// Copyright Epic Games, Inc. All Rights Reserved.

#include "FluidSynthUE.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFluidSynthUEModule"

void FFluidSynthUEModule::StartupModule()
{
}

void FFluidSynthUEModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFluidSynthUEModule, FluidSynth)
