// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

#include "Net/UnrealNetwork.h"

void ATFDGameState::MarkPlayerReady(ATFDPlayerState* PS)
{
	if (PS)
	{
		ReadyPlayers.AddUnique(PS);
	}
}

int32 ATFDGameState::GetReadyPlayerCount() const
{
	return ReadyPlayers.Num();
}

// Client 전용 처리 UI 표시 등에 사용.
void ATFDGameState::OnRep_GameStateChange()
{
	// 클라이언트에서 실행되는 후처리
	switch (GameState)
	{
	case EGameState::WaitingToPlay:
		// 대기 UI 표시, 입력 비활성화 등
		break;
	case EGameState::Result:
		// 로딩 UI 표시
		break;
	case EGameState::Playing:
		// 게임 시작 처리: 이동 허용, AI 시작, UI 닫기 등
		break;
	default:
		break;
	}
}

void ATFDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDGameState, GameState)
	DOREPLIFETIME(ATFDGameState, ReadyPlayers)
}

float ATFDGameState::GetCurrentGameTimeSec()
{
	return 0.f;
}

EGameState ATFDGameState::GetCurrentGameState()
{
	return EGameState::WaitingToPlay;
}
