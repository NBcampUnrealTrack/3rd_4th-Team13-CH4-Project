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
enum class EGameCompleteType : uint8
{
	CatchAllThief,
	TimeLimit,
	ThiefWinByScore,
};