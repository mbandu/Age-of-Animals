using UnrealBuildTool;

public class AgeOfAnimals : ModuleRules
{
	public AgeOfAnimals(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Latest;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"Paper2D",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"OnlineServicesInterface",
			"Slate",
			"SlateCore",
			"UMG",
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"EnhancedInput",
			"DeveloperSettings",
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
