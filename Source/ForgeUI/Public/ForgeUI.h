#pragma once

#include "Modules/ModuleManager.h"

class FForgeUIModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};
