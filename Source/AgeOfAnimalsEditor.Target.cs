using UnrealBuildTool;
using System.Collections.Generic;

public class AgeOfAnimalsEditorTarget : TargetRules
{
	public AgeOfAnimalsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.Add("AgeOfAnimals");
		ExtraModuleNames.Add("AgeOfAnimalsEditor");
		CppStandard = CppStandardVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
	}
}



