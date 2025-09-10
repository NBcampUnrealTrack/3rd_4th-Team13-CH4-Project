// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameData/EGameEnums.h"
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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostSeamlessTravel() override;

	// AI 군중 배치 및 리플리케이트 
	void SpawnAICrowd();
	// 플레이어들 위치 배치와 연결
	void SetPlayerPositions();

	void OnCatchThief(APawn* APawn); //TODO: 도둑 폰으로 클래스 교체
	
	//  게임 시작 전, 결과 표시중 플레이어 조종 방지처리
	void GamePause();
	// 게임 종료시 처리될 내용이 담김.
	void OnHandleGameEnd(EGameCompleteType CompleteType);
	
};
