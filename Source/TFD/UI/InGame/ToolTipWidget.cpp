// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/ToolTipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "PlayerState/TFDPlayerState.h"
#include "TFDNativeGameplayTags.h"

void UToolTipWidget::SetupForTeam(FGameplayTag TeamTag)
{
    if (TeamTag == TAG_Team_Cop)
    {
        // 텍스트 적용
        TeamObjectiveText->SetText(PoliceObjectiveText);
        TeamAbilityText->SetText(PoliceAbilityText);
        ExtraDescriptionText->SetText(PoliceExtraText);

        // 이미지 적용
        if (PoliceIconTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(PoliceIconTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamIcon->SetBrush(Brush);
        }
        if (PoliceObjectiveTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(PoliceObjectiveTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamObjectiveImage->SetBrush(Brush);
        }
        if (PoliceAbilityTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(PoliceAbilityTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamAbilityImage->SetBrush(Brush);
        }
    }
    else if(TeamTag == TAG_Team_Thief)
    {
        // 텍스트 적용
        TeamObjectiveText->SetText(ThiefObjectiveText);
        TeamAbilityText->SetText(ThiefAbilityText);
        ExtraDescriptionText->SetText(ThiefExtraText);

        // 이미지 적용
        if (ThiefIconTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(ThiefIconTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamIcon->SetBrush(Brush);
        }
        if (ThiefObjectiveTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(ThiefObjectiveTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamObjectiveImage->SetBrush(Brush);
        }
        if (ThiefAbilityTex)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(ThiefAbilityTex);
            Brush.ImageSize = FVector2D(PoliceIconTex->GetSourceSize().X, PoliceIconTex->GetSourceSize().Y);

            TeamAbilityImage->SetBrush(Brush);
        }
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
