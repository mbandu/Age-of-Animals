using UnrealBuildTool;

public class AgeOfAnimalsEditor : ModuleRules
{
	public AgeOfAnimalsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Latest;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.Add("AgeOfAnimals");
	}
}
