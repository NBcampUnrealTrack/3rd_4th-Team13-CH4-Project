// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UW_TeamSelection.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UUW_TeamSelection : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Police;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Thief;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MsgTextBlock;

private:
    UFUNCTION()
    void OnPoliceButtonClicked();

    UFUNCTION()
    void OnThiefButtonClicked();
};
