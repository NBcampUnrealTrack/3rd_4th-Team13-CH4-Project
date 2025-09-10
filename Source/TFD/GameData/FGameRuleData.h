#pragma once

#include "CoreMinimal.h"
#include "GameData/EGameEnums.h"
#include "FGameRuleData.generated.h"


USTRUCT(BlueprintType)
struct FGameRuleData
{
	GENERATED_BODY()

	float PlayTimeSec = 90.f;
	float PoliceRatio = 0.5f;		//경찰 비율 올림으로 계산 최소 1명 최대 2명

	TArray<TSoftObjectPtr<AActor*>> PlayerTransformThief;
	TArray<TSoftObjectPtr<AActor*>> PlayerTransformPolice;

	//TSubclassOf
	// 캐릭터 액터 에셋
	// 군중
	// 도둑
	// 경찰 액터
	
};
