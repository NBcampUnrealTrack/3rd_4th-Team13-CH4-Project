// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TFDGameMode.generated.h"

/**
 * 게임 룰 관리자.
 */
UCLASS()
class TFD_API ATFDGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	

public:
	bool IsGameDone();	// 도둑이 다 잡혔거나 시간이 완료됨

	
	// AI 군중 배치 및 리플리케이트 
	void SpawnAICrowd();
	// 플레이어들 위치 배치와 연결
	void SetPlayerPositions();
	//  게임 시작 전, 결과 표시중 플레이어 조종 방지처리
	void GamePause();
	
};
