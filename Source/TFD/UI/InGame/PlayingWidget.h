// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayingWidget.generated.h"


class UTextBlock;
class ATFDGameState;
class ATFDPlayerState;

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
    void UpdateRemainingTime();
protected:
    float StartGameSec;
    float TotalGameSec;

    FTimerHandle RemainingTimeHandle;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RemainingTimeText;

private:
    ATFDGameState* CachedGameState = nullptr;

    UPROPERTY(meta = (BindWidget))
    class UMatchStateWidget* MatchStateWidget = nullptr;
};
