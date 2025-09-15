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

    /** HUD 업데이트 */
    UFUNCTION()
    void UpdateFromGameState();

    UFUNCTION(BlueprintCallable)
    void UpdateThiefScore(int32 NewScore);

    UFUNCTION(BlueprintCallable)
    void UpdateThiefCount();

    UFUNCTION(BlueprintCallable)
    void UpdateRemainingTime(float RemainingTimeSec);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RemainingTimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ThiefGoalText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ThiefCountText;

private:
    ATFDGameState* CachedGameState = nullptr;
};
