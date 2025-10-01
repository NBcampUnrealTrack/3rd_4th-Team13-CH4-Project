// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperSprite.h"
#include "ToolTipWidget.generated.h"

class ATFDPlayerState;
/**
 * 
 */
UCLASS()
class TFD_API UToolTipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // UMG에서 바인딩할 위젯들
    UPROPERTY(meta = (BindWidget))
    class UImage* TeamIcon;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TeamObjectiveText;

    UPROPERTY(meta = (BindWidget))
    class UImage* TeamObjectiveImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TeamAbilityText;

    UPROPERTY(meta = (BindWidget))
    class UImage* TeamAbilityImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ExtraDescriptionText;

    // 초기화 함수
    UFUNCTION(BlueprintCallable)
    void SetupForTeam(FGameplayTag TeamTag);

    virtual void NativeConstruct() override;

    void SetOwnerPawn(APawn* InPawn);

    void TryInitializeOwnerPawnState();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText PoliceObjectiveText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText PoliceAbilityText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText PoliceExtraText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText ThiefObjectiveText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText ThiefAbilityText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Text", meta = (MultiLine = true))
    FText ThiefExtraText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> PoliceIconTex = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> PoliceObjectiveTex = nullptr;

    // Texture2D 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> PoliceAbilityTex = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> ThiefIconTex = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> ThiefObjectiveTex = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamUI|Image")
    TObjectPtr<UPaperSprite> ThiefAbilityTex = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "ToolTip")
    APawn* OwnerPawn;

    UPROPERTY(BlueprintReadWrite, Category = "MiniMap")
    ATFDPlayerState* OwnerPawnState;
};
