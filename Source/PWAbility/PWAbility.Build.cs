// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

using UnrealBuildTool;

public class PWAbility : ModuleRules
{
	public PWAbility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "PWSharedTypes", "PWContracts",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"Niagara",
				"PWContracts", 
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		PrivateDependencyModuleNames.Add("PWTeams");
		PrivateDependencyModuleNames.Add("GameplayTags");
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
