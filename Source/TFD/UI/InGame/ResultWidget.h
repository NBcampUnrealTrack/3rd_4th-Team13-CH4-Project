// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

class ATFDPlayerState;
class ATFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class TFD_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitResult(FGameplayTag WinTeamTag, EGameCompleteType InCompleteType, ATFDPlayerCharacter* InOwnerCharacter);

    UPROPERTY()
    ATFDPlayerState* CachedPlayerState;

    UPROPERTY()
    ATFDPlayerCharacter* OwnerCharacter;
protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ResultText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ConditionText;
};
