// Copyright 2024 Andrew Bindraw. All Rights Reserved.

#include "SystemMicControlLite.h"
#include "SystemMicLiteManager.h"

#define LOCTEXT_NAMESPACE "FSystemMicControlLiteModule"

void FSystemMicControlLiteModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSystemMicLiteManager::Get();
}

void FSystemMicControlLiteModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FSystemMicLiteManager::Get()->DestroyInstance();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSystemMicControlLiteModule, SystemMicControlLite)