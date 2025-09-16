// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_TeamSelection.h"
#include "Controller/TFDPlayerController.h"
#include "TFDNativeGameplayTags.h"
#include "Components/TextBlock.h"


void UUW_TeamSelection::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_Police)
    {
        Btn_Police->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnPoliceButtonClicked);
    }
    if (Btn_Thief)
    {
        Btn_Thief->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnThiefButtonClicked);
    }
}

void UUW_TeamSelection::OnPoliceButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("prefer police"));

    if (MsgTextBlock)
    {
        MsgTextBlock->SetText(FText::FromString(TEXT("Prefer Police")));
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ATFDPlayerController* TFDPC = Cast<ATFDPlayerController>(PC))
        {
            TFDPC->SendPreferredTeam(TAG_Team_Cop);
        }
    }
}

void UUW_TeamSelection::OnThiefButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("prefer thief"));

    if (MsgTextBlock)
    {
        MsgTextBlock->SetText(FText::FromString(TEXT("Prefer Thief")));
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ATFDPlayerController* TFDPC = Cast<ATFDPlayerController>(PC))
        {
            TFDPC->SendPreferredTeam(TAG_Team_Thief);
        }
    }
}