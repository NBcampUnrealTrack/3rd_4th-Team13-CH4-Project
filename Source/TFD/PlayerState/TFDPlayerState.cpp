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

	//팀 희망 변수 복제 등록
	DOREPLIFETIME(ATFDPlayerState, PreferredTeam);
	//실제 팀 변수 복제 등록
	DOREPLIFETIME(ATFDPlayerState, ActualTeam);
}


// PreferredTeam 관련
// 선호 팀 설정/획득 함수
FGameplayTag ATFDPlayerState::GetPreferredTeam() const
{
	return PreferredTeam;
}

void ATFDPlayerState::SetPreferredTeam(const FGameplayTag& InTeamTag)
{
	PreferredTeam = InTeamTag;
}

// ActualTeam 관련
void ATFDPlayerState::SetActualTeam(const FGameplayTag& NewTeamTag)
{
	if (ActualTeam != NewTeamTag)
	{
		ActualTeam = NewTeamTag;
		// 서버에서 변경 후 클라이언트에 동기화
	}
}

// 실제 팀 획득 함수
FGameplayTag ATFDPlayerState::GetActualTeam() const
{
	return ActualTeam;
}

// ActualTeam 복제 완료 시 호출됨 (클라이언트)
void ATFDPlayerState::OnRep_ActualTeam()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s replicated ActualTeam: %s"),
		*GetPlayerName(),
		*ActualTeam.GetTagName().ToString());

	// UI 업데이트 등 추가 작업
}