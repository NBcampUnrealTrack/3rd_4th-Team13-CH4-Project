// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_TeamSelection.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Controller/TFDPlayerController.h"
#include "PlayerState/TFDPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UUW_TeamSelection::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_Cop)
    {
        Btn_Cop->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnClickTeamCop);
    }
    if (Btn_Thief)
    {
        Btn_Thief->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnClickTeamThief);
    }
}

void UUW_TeamSelection::OnClickTeamCop()
{
    if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
    {
        PC->SendPreferredTeam(TAG_Team_Cop);
        UpdateSelectedTeamText(TAG_Team_Cop);
    }
}

void UUW_TeamSelection::OnClickTeamThief()
{
    if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
    {
        PC->SendPreferredTeam(TAG_Team_Thief);
        UpdateSelectedTeamText(TAG_Team_Thief);
    }
}

void UUW_TeamSelection::UpdateSelectedTeamText(FGameplayTag NewTeam)
{
    if (NewTeam == TAG_Team_Cop && Txt_Cop)
    {
        Txt_Cop->SetText(FText::FromString("Selected"));
        if (Txt_Thief)
        {
            Txt_Thief->SetText(FText::FromString(""));
        }
    }
    else if (NewTeam == TAG_Team_Thief && Txt_Thief)
    {
        Txt_Thief->SetText(FText::FromString("Selected"));
        if (Txt_Cop)
        {
            Txt_Cop->SetText(FText::FromString(""));
        }
    }
}