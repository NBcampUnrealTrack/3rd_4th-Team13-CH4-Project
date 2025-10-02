// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/ToolTipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameInstance/TFDGameInstance.h"
#include "PlayerState/TFDPlayerState.h"
#include "TFDNativeGameplayTags.h"

void UToolTipWidget::SetupForTeam(FGameplayTag TeamTag)
{
    auto* GI = Cast<UTFDGameInstance>(GetGameInstance());

    check(GI)


    EUIIconType TeamIconType = (TeamTag == TAG_Team_Cop) ? EUIIconType::Cops : EUIIconType::Thief;
    GI->RequestUIIcon(TeamIconType, [this](const TObjectPtr<UPaperSprite>& Sprite)
    {
        if (Sprite && TeamIcon)
        {
            const FVector2D& Size = TeamIcon->GetDesiredSize();
            FSlateBrush Brush = UUIResourceAsset::MakeBrushFromSprite(Sprite, Size.X, Size.Y);
            TeamIcon->SetBrush(Brush);
        }
    });

    EUIIconType TeamObjectiveImageType = (TeamTag == TAG_Team_Cop) ? EUIIconType::Jail : EUIIconType::Gold;
    GI->RequestUIIcon(TeamObjectiveImageType, [this](const TObjectPtr<UPaperSprite>& Sprite)
    {
        if (Sprite && TeamIcon)
        {
            const FVector2D& Size = TeamIcon->GetDesiredSize();
            FSlateBrush Brush = UUIResourceAsset::MakeBrushFromSprite(Sprite, Size.X, Size.Y);
            TeamObjectiveImage->SetBrush(Brush);
        }
    });

    EUIIconType TeamAbilityImageType = (TeamTag == TAG_Team_Cop) ? EUIIconType::Handcuff : EUIIconType::Exit;
    GI->RequestUIIcon(TeamAbilityImageType, [this](const TObjectPtr<UPaperSprite>& Sprite)
    {
        if (Sprite && TeamIcon)
        {
            const FVector2D& Size = TeamIcon->GetDesiredSize();
            FSlateBrush Brush = UUIResourceAsset::MakeBrushFromSprite(Sprite, Size.X, Size.Y);
            TeamAbilityImage->SetBrush(Brush);
        }
    });

    if (TeamTag == TAG_Team_Cop)
    {
        // 텍스트 적용
        TeamObjectiveText->SetText(PoliceObjectiveText);
        TeamAbilityText->SetText(PoliceAbilityText);
        ExtraDescriptionText->SetText(PoliceExtraText);
    }
    else if(TeamTag == TAG_Team_Thief)
    {
        // 텍스트 적용
        TeamObjectiveText->SetText(ThiefObjectiveText);
        TeamAbilityText->SetText(ThiefAbilityText);
        ExtraDescriptionText->SetText(ThiefExtraText);
    }
}

void UToolTipWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UToolTipWidget::SetOwnerPawn(APawn* InPawn)
{
    OwnerPawn = InPawn;
    TryInitializeOwnerPawnState();
}

void UToolTipWidget::TryInitializeOwnerPawnState()
{
    if (!OwnerPawn) return;

    OwnerPawnState = OwnerPawn->GetPlayerState<ATFDPlayerState>();
    if (!OwnerPawnState)
    {
        // PawnState 아직 없으면 다음 Tick에 재시도
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UToolTipWidget::TryInitializeOwnerPawnState);
        return;
    }

    FGameplayTag TeamTag = OwnerPawnState->GetActualTeam();
    SetupForTeam(TeamTag);
}
