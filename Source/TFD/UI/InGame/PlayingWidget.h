// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayingWidget.generated.h"


class UTextBlock;
class ATFDGameState;

/**
 * 
 */
UCLASS()
class TFD_API UPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable)
    void UpdateThiefScore(int32 NewScore);

    UFUNCTION(BlueprintCallable)
    void UpdateThiefCount();

    UFUNCTION(BlueprintCallable)
    void UpdateRemainingTime();

    UFUNCTION(BlueprintCallable)
    void UpdateTeamName();

protected:
    float StartGameSec;
    float TotalGameSec;

    FTimerHandle RemainingTimeHandle;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RemainingTimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ThiefGoalText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ThiefCountText;

    /** 팀 이름 출력용 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamNameText;


private:
    ATFDGameState* CachedGameState = nullptr;
};
