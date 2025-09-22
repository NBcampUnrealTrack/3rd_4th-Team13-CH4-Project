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
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    CachedGameState = PC->GetWorld()->GetGameState<ATFDGameState>();
    if (!CachedGameState) return;
    
    UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] CachedGameState found: %s"), *CachedGameState->GetName());
    // 델리게이트 바인딩
    CachedGameState->OnThiefScoreChanged.AddDynamic(this, &UPlayingWidget::UpdateThiefScore);
    CachedGameState->OnThievesChanged.AddDynamic(this, &UPlayingWidget::UpdateThiefCount);
    StartGameSec = CachedGameState->GetServerWorldTimeSeconds();
    TotalGameSec = CachedGameState->GetRuleData()->PlayTimeSec;
    
    UpdateThiefCount();
    UpdateThiefScore(0);
    UpdateRemainingTime();
    UpdateTeamName();
    
    GetWorld()->GetTimerManager().SetTimer(
        RemainingTimeHandle,
        this,
        &UPlayingWidget::UpdateRemainingTime,
        1.0f,
        true
    );
    
}

void UPlayingWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RemainingTimeHandle);
    }
}

void UPlayingWidget::UpdateThiefScore(int32 NewScore)
{
    int32 TotalScoreGoal = CachedGameState->GetRuleData()->ThiefScoreForWin;

    ThiefGoalText->SetText(FText::FromString(
        FString::Printf(TEXT("현재 점수: %d / 총 점수: %d"), NewScore, TotalScoreGoal)
    ));
}

void UPlayingWidget::UpdateThiefCount()
{
    if (!CachedGameState) return;

    int32 RemainingThieves = CachedGameState->ThiefPlayerStateArray.Num();
    int32 CaughtThieves = CachedGameState->CaughtThiefPlayerStateArray.Num();

    if (!ThiefCountText) return;
    
    ThiefCountText->SetText(FText::FromString(FString::Printf(TEXT("총 도둑 수: %d / 잡힌 도둑 수: %d"), RemainingThieves, CaughtThieves)));
}

void UPlayingWidget::UpdateRemainingTime()
{


    float RemainGameSec = TotalGameSec - (CachedGameState->GetServerWorldTimeSeconds() - StartGameSec);

    int32 RemainMinutes = FMath::FloorToInt(RemainGameSec / 60.f);
    int32 RemainSeconds = FMath::FloorToInt(RemainGameSec) % 60;

    if (!RemainingTimeText) return;
    
    RemainingTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), RemainMinutes, RemainSeconds)));
}

void UPlayingWidget::UpdateTeamName()
{
    ATFDPlayerState* PS = GetOwningPlayer()->GetPlayerState<ATFDPlayerState>();
    if (!PS)
    {
        // PlayerState가 아직 없으면 다음 Tick에 재시도
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                UpdateTeamName();
            });
        return;
    }

    FGameplayTag TeamTag = PS->GetTeamTag();

    if (!TeamTag.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                UpdateTeamName();
            });
        return;
    }

        // 팀 이름 바로 업데이트
    if (!TeamNameText) return;
    
    TeamNameText->SetText(FText::FromString(FString::Printf(TEXT("팀: %s"), *TeamTag.ToString())));
}
