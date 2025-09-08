// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/FGameRuleData.h"
#include "GameFramework/GameState.h"
#include "TFDGameState.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDGameState : public AGameState
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly);
	FGameRuleData GameRuleData;		//TODO: DA 로 변경

	float GameStartServerTime = 0.f;

public:

	float GetCurrentGameTimeSec();
};


enum class EGameState : uint8
{
	Ready,	// 준비
	Run,	// 진행 중
	Result	// 결과 표시 재시작?
};