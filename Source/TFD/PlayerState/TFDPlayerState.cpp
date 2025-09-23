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
		NewPS->PreferredTeam = PreferredTeam;
		NewPS->ActualTeam = ActualTeam;
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
	DOREPLIFETIME(ATFDPlayerState, TeamTag);
	DOREPLIFETIME(ATFDPlayerState, PreferredTeam);
	DOREPLIFETIME(ATFDPlayerState, ActualTeam);
}

FGameplayTag ATFDPlayerState::GetPreferredTeam() const
{
	return PreferredTeam;
}

void ATFDPlayerState::SetPreferredTeam(const FGameplayTag& InTeamTag)
{
	PreferredTeam = InTeamTag;
}

FGameplayTag ATFDPlayerState::GetActualTeam() const
{
	return ActualTeam;
}

void ATFDPlayerState::SetActualTeam(const FGameplayTag& InTeam)
{
	if (ActualTeam != InTeam)
	{
		ActualTeam = InTeam;
		UE_LOG(LogTemp, Log, TEXT("Player %s assigned ActualTeam: %s (Server)"),
			*GetPlayerName(),
			*ActualTeam.GetTagName().ToString());
	}
}

void ATFDPlayerState::OnRep_ActualTeam()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s replicated ActualTeam: %s"),
		*GetPlayerName(),
		*ActualTeam.GetTagName().ToString());

	// UI 업데이트 등 필요한 작업 여기에 추가 가능
}