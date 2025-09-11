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
				"NavigationSystem",

				// IP 관련 내용
				"Http",
                "Json",			// (선택적 - JSON 파싱 시 필요)
                "JsonUtilities"	// (선택적 - JSON 구조체 바인딩 시 필요)
        });
		
		//PublicIncludePaths.AddRange(new string[] { "TFD" });

		PublicIncludePaths.AddRange(new string[] { 
			ModuleDirectory + "/../TFD"
		});
		
		
	}
}
