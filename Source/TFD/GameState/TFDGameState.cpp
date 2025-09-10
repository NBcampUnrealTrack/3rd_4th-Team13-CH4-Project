// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

#include "Net/UnrealNetwork.h"

void ATFDGameState::MarkPlayerReady(ATFDPlayerState* PS)
{
	ReadyPlayers.Add(PS);

	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		Multicast_StartGame();
	}
}

void ATFDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDGameState, GameState)
	DOREPLIFETIME(ATFDGameState, ReadyPlayers)
}

void ATFDGameState::Multicast_StartGame_Implementation()
{
	// 여기서 조작 허용, UI 닫기 등
	UE_LOG(LogTemp, Log, TEXT("모든 플레이어 준비 완료 -> 게임 시작!"));
}
