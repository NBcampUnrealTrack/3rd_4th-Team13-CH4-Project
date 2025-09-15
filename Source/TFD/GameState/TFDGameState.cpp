// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/TFDGameState.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"

#include "UI/GameUIRouterSubsystem.h"
#include "UI/InGame/PlayingWidget.h"
#include "UI/Widget/UHUDLayoutWidget.h"

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

	DOREPLIFETIME(ATFDGameState, GameRemainServerTime);
	DOREPLIFETIME(ATFDGameState, ThiefTotalScore);
	DOREPLIFETIME(ATFDGameState, CaughtThiefPlayerStateArray);
}

float ATFDGameState::GetCurrentGameTimeSec() const
{
	return GetServerWorldTimeSeconds() - GameRemainServerTime;
}

// Client 전용 처리 UI 표시 등에 사용.
void ATFDGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	APlayerController* PC = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());
	if (!PC || !PC->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnRep_MatchState] Not local controller, returning."));
		return; // 로컬 클라이언트가 아니면 더 이상 진행하지 않음
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
		PC->bShowMouseCursor = false;
		// 게임 시작 UI 처리
		if (UGameUIRouterSubsystem* UISub = PC->GetLocalPlayer()->GetSubsystem<UGameUIRouterSubsystem>())
		{
			if (HUDWidgetClass)
			{
				// HUDLayoutClass 세팅 후 생성
				UISub->SetHUDLayoutClass(HUDWidgetClass);
				UISub->CreateHUD();

				// PlayingWidgetClass 추가
				if (PlayingWidgetClass)
				{
					UISub->AddWidgetToLayer(EUILayer::GameLayer, PlayingWidgetClass);
				}
			}
		}
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

void ATFDGameState::OnRep_GameRemainTime()
{
	OnGameTimeChanged.Broadcast(GameRemainServerTime);
}

