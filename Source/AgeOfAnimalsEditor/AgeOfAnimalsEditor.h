#pragma once

#include "Modules/ModuleManager.h"

class FAgeOfAnimalsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
