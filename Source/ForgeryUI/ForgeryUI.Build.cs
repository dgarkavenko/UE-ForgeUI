using System.IO;
using UnrealBuildTool;

public class ForgeryUI : ModuleRules
{
	public ForgeryUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"Slate",
			"SlateCore",
			"CommonUI",
			"CommonInput",
			"CommonGame",
			"GameplayTags"
		});

		// AngelScript reads these off disk at startup (FAngelscriptManager scans
		// <PluginBaseDir>/Script), so they must stage loose rather than into the pak.
		// The project's DirectoriesToAlwaysStageAsNonUFS does not reach plugin paths.
		RuntimeDependencies.Add(
			Path.Combine(PluginDirectory, "Script", "*.as"),
			StagedFileType.NonUFS);
	}
}
