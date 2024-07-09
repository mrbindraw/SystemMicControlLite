// Copyright 2024 Andrew Bindraw. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSystemMicControlLiteModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
