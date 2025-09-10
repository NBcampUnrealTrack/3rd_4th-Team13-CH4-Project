// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TFD : ModuleRules
{
	public TFD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput"
				
				// UI
				,"UMG", "Slate", "SlateCore", 
			    //GAS
				"GameplayAbilities",      // 추가!
                "GameplayTags",           // 추가!
                "GameplayTasks",           // 추가!

				"AIModule",
				"NavigationSystem"
        });
		
		//PublicIncludePaths.AddRange(new string[] { "TFD" });

		PublicIncludePaths.AddRange(new string[] { 
			ModuleDirectory + "/../TFD"
		});
		
		
	}
}
