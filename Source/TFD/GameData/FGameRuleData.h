#pragma once

USTRUCT(BlueprintType)
struct FGameRuleData
{
	float GameTimeSec = 90.f;
	float CapsRatio = 0.5f;		//경찰 비율 올림으로 계산 최소 1명 최대 2명
};
