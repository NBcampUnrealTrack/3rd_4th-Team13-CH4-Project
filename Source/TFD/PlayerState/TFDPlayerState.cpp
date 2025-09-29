// Fill out your copyright notice in the Description page of Project Settings.


#include "TFDPlayerState.h"

#include "Net/UnrealNetwork.h"

void ATFDPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);
	if (ATFDPlayerState* NewPS = Cast<ATFDPlayerState>(NewPlayerState))
	{
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

void ATFDPlayerState::ServerSetNickname_Implementation(const FString& NewNickname)
{
    Nickname = NewNickname;
    SetPlayerName(NewNickname); // ✅ PlayerName도 같이 설정
    OnRep_Nickname(); // 서버에서도 즉시 반영
}


void ATFDPlayerState::OnRep_Nickname()
{
	UE_LOG(LogTemp, Log, TEXT("Nickname replicated: %s"), *Nickname);

	// 필요하면 UI 위젯에 닉네임 전달하거나 델리게이트 호출 가능
	// 예: OnPlayerNameChanged.Broadcast(); 또는 HUD에 표시
}


void ATFDPlayerState::OnRep_ActualTeam()
{
	// 클라이언트 실제 팀 동기화 시 작동 (UI 등 갱신 가능)
	UE_LOG(LogTemp, Log, TEXT("Player %s ActualTeam replicated: %s"),
		*GetPlayerName(),
		*ActualTeam.GetTagName().ToString());
}

void ATFDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDPlayerState, ReplicatedPlayerName);
	DOREPLIFETIME(ATFDPlayerState, PreferredTeam);
	DOREPLIFETIME(ATFDPlayerState, ActualTeam);
}