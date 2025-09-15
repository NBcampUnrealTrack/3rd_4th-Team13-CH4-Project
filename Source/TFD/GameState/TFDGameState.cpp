// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"

ATFDGameState::ATFDGameState()
{
	bReplicates = true;
}

void ATFDGameState::MarkPlayerReady(ATFDPlayerState* PS)
{
	if (PS)
	{
		ReadyPlayers.Add(PS);
	}
}

int32 ATFDGameState::GetReadyPlayerCount() const
{
	return ReadyPlayers.Num();
}

const FGameRuleData& ATFDGameState::GetRuleData() const
{
	return GameRuleData;
}

void ATFDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDGameState, GameStartServerTime)
}

float ATFDGameState::GetCurrentGameTimeSec() const
{
	return GetServerWorldTimeSeconds() - GameStartServerTime;
}

// Client 전용 처리 UI 표시 등에 사용.
void ATFDGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (HasAuthority() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("OnRep_MatchState 호출 실패: 클라이언트의 호출은 막음"));
		return;
	}

	if (MatchState == MatchState::EnteringMap) // 맵 진입 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: EnteringMap"));
		//EnteringMap는 OnRep_MatchState를 호출할수없음... 그냥 초기상태임
	}
	else if (MatchState == MatchState::WaitingToStart) // 게임 시작 전 대기 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: WaitingToStart"));
		// 대기 UI 표시, 입력 비활성화 등
	}
	else if (MatchState == MatchState::InProgress) //실제 게임 시작 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: InProgress"));
		GameStartServerTime = GetServerWorldTimeSeconds();
		// 게임 시작 UI 처리
	}
	else if (MatchState == MatchState::WaitingPostMatch) // 게임 결과 후 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: WaitingPostMatch"));
		// 결과 UI 처리
	}
	else if (MatchState == MatchState::LeavingMap) // 맵을 떠나는 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: LeavingMap"));
		// 세션종료, 레벨 전환 준비 UI 처리
	}
	else if (MatchState == MatchState::Aborted) // 강제 종료 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: Aborted"));
		// 정상적인 종료가 아닌, 강제로 경기가 중단된 상태일때 UI 처리
	}
}