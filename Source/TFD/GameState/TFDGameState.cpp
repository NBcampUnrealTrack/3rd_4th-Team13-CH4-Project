// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

void ATFDGameState::MarkPlayerReady(ATFDPlayerState* PS)
{
	ReadyPlayers.Add(PS);

	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		Multicast_StartGame();
	}
}

void ATFDGameState::Multicast_StartGame_Implementation()
{
	// 여기서 조작 허용, UI 닫기 등
	UE_LOG(LogTemp, Log, TEXT("모든 플레이어 준비 완료 -> 게임 시작!"));
}