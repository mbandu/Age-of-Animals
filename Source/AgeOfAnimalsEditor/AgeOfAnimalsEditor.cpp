#include "AgeOfAnimalsEditor.h"
#include "AoALevelEditorTool.h"

#define LOCTEXT_NAMESPACE "FAgeOfAnimalsEditorModule"

void FAgeOfAnimalsEditorModule::StartupModule()
{
	// Register level editor toolbar buttons for map editing
	UAoALevelEditorTool::RegisterToolbar();
}

void FAgeOfAnimalsEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
