// Fill out your copyright notice in the Description page of Project Settings.


#include "TFDPlayerState.h"

#include "Net/UnrealNetwork.h"

void ATFDPlayerState::SetTeamTag(FGameplayTag Tag)
{
	this->TeamTag = Tag;
}

FGameplayTag ATFDPlayerState::GetTeamTag() const
{
	return this->TeamTag;
}

void ATFDPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);
	if (ATFDPlayerState* NewPS = Cast<ATFDPlayerState>(NewPlayerState))
	{
		NewPS->TeamTag = TeamTag;
	}
}

//===================================================
// 이하 OutGame 관련 - Lobby
//===================================================
ATFDPlayerState::ATFDPlayerState()
{
	// 복제 활성화
	bReplicates = true;
}

void ATFDPlayerState::SetPlayerName(const FString& NewName)
{
	if (HasAuthority())
	{
		ReplicatedPlayerName = NewName;
		// 부모 클래스 PlayerName도 같이 세팅
		Super::SetPlayerName(NewName);
	}
}

void ATFDPlayerState::OnRep_PlayerName()
{
	// 이름이 변경되었을 때 UI 갱신이나 로직 처리할 수 있도록 델리게이트 호출
	OnPlayerNameChanged.Broadcast();
}

void ATFDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDPlayerState, ReplicatedPlayerName);
}