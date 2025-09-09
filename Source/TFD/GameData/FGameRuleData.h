#pragma once

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Spector,
	Police,
	Thief,
	AICrowd,
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	WaitingToPlay,	// 준비
	Playing,		// 진행 중
	Result			// 결과 표시 재시작?
};

UENUM(BlueprintType)
enum class EGameCompleteType : uint8
{
	CatchAllThief,
	TimeLimit,
};

USTRUCT(BlueprintType)
struct FGameRuleData
{
	float PlayTimeSec = 90.f;
	float PoliceRatio = 0.5f;		//경찰 비율 올림으로 계산 최소 1명 최대 2명

	const int MinimumPlayer = 2;

	// 캐릭터 액터 에셋
	// 군중
	// 도둑
	// 경찰 액터
	
};
