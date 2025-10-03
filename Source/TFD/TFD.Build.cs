// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
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
                "Niagara", //나이아가라 추가
				"AIModule",
				"NavigationSystem",
                "Paper2D",

				// IP 관련 내용
				"HTTP",				// 대문자로 수정! ("Http") // Module 'Http' (referenced via Target -> TFD.Build.cs) has incorrect text case. Did you mean 'HTTP'?
                "Json",				// (선택적 - JSON 파싱 시 필요)
                "JsonUtilities",	// (선택적 - JSON 구조체 바인딩 시 필요)
                
                "Paper2D"			//UIIcon 용
        });
		
		//PublicIncludePaths.AddRange(new string[] { "TFD" });

		PublicIncludePaths.AddRange(new string[] { 
			ModuleDirectory + "/../TFD",
			// Engine 의 경로는 팀원마다 상이할 수 있습니다
			// 에러를 일이키는 것은 아니지만 해당 폴더를 찾을 수 없다는 에러가 발생합니다.
			// 제거 후 빌드와 테스트를 통과 함으로 주석 처리 했습니다.
            //Path.Combine(ModuleDirectory, "../../Engine/Plugins/2D/Paper2D/Source/Paper2D/Public")
        });
		
		
	}
}
