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