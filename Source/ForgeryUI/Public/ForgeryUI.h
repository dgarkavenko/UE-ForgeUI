#pragma once

#include "Modules/ModuleManager.h"

class FForgeryUIModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};
