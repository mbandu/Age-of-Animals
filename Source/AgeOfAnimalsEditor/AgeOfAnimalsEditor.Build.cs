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
			"UnrealEd",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"PropertyEditor",
			"LevelEditor",
			"AssetTools",
			"AssetRegistry",
			"ContentBrowser",
			"Json",
			"JsonUtilities",
			"InputCore",
			"ToolMenus",
		});

		PrivateDependencyModuleNames.Add("AgeOfAnimals");
	}
}
