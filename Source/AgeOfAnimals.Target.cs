using UnrealBuildTool;
using System.Collections.Generic;

public class AgeOfAnimalsTarget : TargetRules
{
	public AgeOfAnimalsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		ExtraModuleNames.Add("AgeOfAnimals");
		CppStandard = CppStandardVersion.Latest;
	}
}
