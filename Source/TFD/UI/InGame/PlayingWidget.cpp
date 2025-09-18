// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/PlayingWidget.h"
#include "Components/TextBlock.h"
#include "GameState/TFDGameState.h"
#include "PlayerState/TFDPlayerState.h"
#include "GameFramework/PlayerController.h"

void UPlayingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] NativeConstruct called."));

    if (APlayerController* PC = GetOwningPlayer())
    {
        UpdateTeamName();


        CachedGameState = PC->GetWorld()->GetGameState<ATFDGameState>();
        if (CachedGameState)
        {
            UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] CachedGameState found: %s"), *CachedGameState->GetName());

            // 델리게이트 바인딩
            CachedGameState->OnThiefScoreChanged.AddDynamic(this, &UPlayingWidget::UpdateThiefScore);
            CachedGameState->OnThievesChanged.AddDynamic(this, &UPlayingWidget::UpdateThiefCount);
            CachedGameState->OnGameTimeChanged.AddDynamic(this, &UPlayingWidget::UpdateRemainingTime);

            // HUD 초기화
            UpdateFromGameState();
        }
    }
}


void UPlayingWidget::UpdateFromGameState()
{

    if (!CachedGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlayingWidget] CachedGameState is null, cannot update HUD."));
        return;
    }
    UpdateThiefCount();
    UpdateThiefScore(0);
}

void UPlayingWidget::UpdateThiefScore(int32 NewScore)
{
    int32 TotalScoreGoal = CachedGameState->GetRuleData().ThiefScoreForWin;

    ThiefGoalText->SetText(FText::FromString(
        FString::Printf(TEXT("현재 점수: %d / 총 점수: %d"), NewScore, TotalScoreGoal)
    ));
}

void UPlayingWidget::UpdateThiefCount()
{
    if (!CachedGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlayingWidget] UpdateThiefCount 호출됨, 하지만 CachedGameState 없음"));
        return;
    }

    int32 RemainingThieves = CachedGameState->ThiefPlayerStateArray.Num();
    int32 CaughtThieves = CachedGameState->CaughtThiefPlayerStateArray.Num();

    if (ThiefCountText)
        ThiefCountText->SetText(FText::FromString(FString::Printf(TEXT("총 도둑 수: %d / 잡힌 도둑 수: %d"), RemainingThieves, CaughtThieves)));
}

void UPlayingWidget::UpdateRemainingTime(float RemainingTimeSec)
{
    int32 Minutes = FMath::FloorToInt(RemainingTimeSec / 60.f);
    int32 Seconds = FMath::FloorToInt(RemainingTimeSec) % 60;

    if (RemainingTimeText)
    {
        RemainingTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }
}

void UPlayingWidget::UpdateTeamName()
{
    if (ATFDPlayerState* PS = GetOwningPlayer()->GetPlayerState<ATFDPlayerState>())
    {
        FGameplayTag TeamTag = PS->GetTeamTag();

        // TeamTag가 None이면 다음 Tick에 재시도
        if (!TeamTag.IsValid())
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    UpdateTeamName();
                });
            return;
        }

        // 팀 이름 바로 업데이트
        if (TeamNameText)
        {
            TeamNameText->SetText(FText::FromString(FString::Printf(TEXT("팀: %s"), *TeamTag.ToString())));
        }
    }
    else
    {
        // PlayerState가 아직 없으면 다음 Tick에 재시도
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                UpdateTeamName();
            });
    }
}
