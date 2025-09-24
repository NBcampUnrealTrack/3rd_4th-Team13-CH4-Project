// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"

#include "UI/GameUIRouterSubsystem.h"
#include "UI/InGame/PlayingWidget.h"
#include "UI/Widget/UHUDLayoutWidget.h"
#include "UI/InGame/ResultWidget.h"

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

UTFDGameRuleData* ATFDGameState::GetRuleData() const
{
	return GameRuleData;
}

void ATFDGameState::AssignTeamsBasedOnPreference()
{
	TArray<ATFDPlayerState*> PreferredPolicePlayers;
	TArray<ATFDPlayerState*> OtherPlayers;
	const int32 PoliceTeamMax = 2;

	// 전체 플레이어를 추출
	for (APlayerState* PS : PlayerArray)
	{
		if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
		{
			if (TFDPS->GetPreferredTeam() == TAG_Team_Cop)
			{
				PreferredPolicePlayers.Add(TFDPS);
			}
			else
			{
				OtherPlayers.Add(TFDPS);
			}
		}
	}

	// 경찰 선호자 수에 따라 팀 배정
	TArray<ATFDPlayerState*> AssignedPolice;
	if (PreferredPolicePlayers.Num() == 0)
	{
		int32 NumToAssign = FMath::Min(PoliceTeamMax, OtherPlayers.Num());
		for (int32 i = 0; i < NumToAssign; ++i)
		{
			int32 RandomIndex = FMath::RandRange(0, OtherPlayers.Num() - 1);
			AssignedPolice.Add(OtherPlayers[RandomIndex]);
			OtherPlayers[RandomIndex]->SetActualTeam(TAG_Team_Cop);
			OtherPlayers.RemoveAt(RandomIndex);
		}
	}
	else if (PreferredPolicePlayers.Num() <= PoliceTeamMax)
	{
		for (ATFDPlayerState* Player : PreferredPolicePlayers)
		{
			Player->SetActualTeam(TAG_Team_Cop);
			AssignedPolice.Add(Player);
		}
	}
	else
	{
		for (int32 i = PreferredPolicePlayers.Num() - 1; i > 0; --i)
		{
			int32 SwapIndex = FMath::RandRange(0, i);
			PreferredPolicePlayers.Swap(i, SwapIndex);
		}
		for (int32 i = 0; i < PoliceTeamMax; ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Cop);
			AssignedPolice.Add(PreferredPolicePlayers[i]);
		}
		for (int32 i = PoliceTeamMax; i < PreferredPolicePlayers.Num(); ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Thief);
			OtherPlayers.Add(PreferredPolicePlayers[i]);
		}
	}

	// 나머지 플레이어 도둑 배정
	for (ATFDPlayerState* Player : OtherPlayers)
	{
		Player->SetActualTeam(TAG_Team_Thief);
	}

	// 로그 출력
	for (APlayerState* PS : PlayerArray)
	{
		if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
		{
			UE_LOG(LogTemp, Log, TEXT("Player %s assigned to team %s"),
				*TFDPS->GetPlayerName(),
				*TFDPS->GetActualTeam().ToString());
		}
	}
}

void ATFDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFDGameState, ThiefTotalScore);
	DOREPLIFETIME(ATFDGameState, PolicePlayerStateArray);
	DOREPLIFETIME(ATFDGameState, ThiefPlayerStateArray);
	DOREPLIFETIME(ATFDGameState, CaughtThiefPlayerStateArray);
	DOREPLIFETIME(ATFDGameState, PoliceMapItemArray);
	DOREPLIFETIME(ATFDGameState, ThiefMapItemArray);
	DOREPLIFETIME(ATFDGameState, WinTeamTag);
	DOREPLIFETIME(ATFDGameState, CompleteType);
}

float ATFDGameState::GetCurrentGameTimeSec() const
{
	return GetServerWorldTimeSeconds() - GameRemainServerTime;
}

// Client 전용 처리 UI 표시 등에 사용.
void ATFDGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

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

		OnMachInProgress.Broadcast();
	}
	else if (MatchState == MatchState::WaitingPostMatch) // 게임 결과 후 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchState: WaitingPostMatch"));
		// 결과 UI 처리
		UE_LOG(LogTemp, Warning, TEXT("Win Team Tag : %s"), *WinTeamTag.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Win CompleteType : %s"), *UEnum::GetValueAsString(CompleteType));

		OnMatchWaitingPostMatch.Broadcast(WinTeamTag, CompleteType);
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

void ATFDGameState::OnRep_CaughtThiefPlayerStateArray()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATFDGameState] OnRep_CaughtThiefPlayerStateArray 호출됨, CaughtThiefPlayerStateArray.Num(): %d"), CaughtThiefPlayerStateArray.Num());
	// 클라에서 델리게이트 발행
	OnThievesChanged.Broadcast();
}


void ATFDGameState::AddThiefScore(int32 Points)
{
	if (HasAuthority())
	{
		//서버에서만 점수 관리, 네트워크가 자동으로 클라이언트에게 ThiefTotalScore를 복제
		ThiefTotalScore += Points;

		// 서버에서 점수 변경 이벤트 발행
		OnThiefScoreChanged.Broadcast(ThiefTotalScore);
	}
}

void ATFDGameState::OnRep_ThiefScore()
{
	if (!HasAuthority())
	{
		// 클라에서도 점수 변경 이벤트 발행 (UI 갱신 가능)
		OnThiefScoreChanged.Broadcast(ThiefTotalScore);
	}
}

void ATFDGameState::OnRep_PolicePlayerStateArray()
{
	OnPoliceArrayChanged.Broadcast(PolicePlayerStateArray);
}

void ATFDGameState::OnRep_ThiefPlayerStateArray()
{
	OnThiefArrayChanged.Broadcast(ThiefPlayerStateArray);
}

void ATFDGameState::OnRep_PoliceMapItemArray()
{
	OnPoliceItemArrayChanged.Broadcast(PoliceMapItemArray);
}

void ATFDGameState::OnRep_ThiefMapItemArray()
{
	OnThiefItemArrayChanged.Broadcast(ThiefMapItemArray);
}

void ATFDGameState::RemoveAllowedItem(ATFDBaseObject* Object)
{
	if (!Object) return;

	const FGameplayTagContainer& ItemTag = Object->GetAllowedTeamTag();

	if (ItemTag.HasTag(TAG_Team_Cop))
	{
		PoliceMapItemArray.RemoveSingleSwap(Object);
	}
	else if (ItemTag.HasTag(TAG_Team_Thief))
	{
		ThiefMapItemArray.RemoveSingleSwap(Object);
	}
}

void ATFDGameState::SetWinTeam(FGameplayTag WinTeam, EGameCompleteType InCompleteType)
{
	WinTeamTag = WinTeam;
	CompleteType = InCompleteType;
}
