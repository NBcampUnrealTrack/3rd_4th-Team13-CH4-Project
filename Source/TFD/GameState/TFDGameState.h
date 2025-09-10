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
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess));
	FGameRuleData GameRuleData;		//TODO: DA 로 변경

	float GameStartServerTime = 0.f;

	UPROPERTY(Replicated)
	EGameState GameState;	// 게임상태 변경처리 OnRep 함수 생성하여 후처리
	
	// 현재 게임 상태필요
	// 도둑정보 Controller 리스트
	// 경찰정보 Controller 리스트

	UPROPERTY(Replicated)
	TArray<ATFDPlayerState*> ReadyPlayers;

	UFUNCTION()
	void MarkPlayerReady(ATFDPlayerState* PS);

	// 게임 시작 알림용 Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartGame();

	
	
public:
	float GetCurrentGameTimeSec();		// 현재 서버시간 - 게임 시작 서버시간 
	EGameState GetCurrentGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const int MinimumPlayerNum = 2;
};