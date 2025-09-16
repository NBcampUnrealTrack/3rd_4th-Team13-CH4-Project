// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/ResultWidget.h"
#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Character/TFDCharacterBase.h"
#include "Components/TextBlock.h"
#include "GameData/EGameEnums.h"
#include "PlayerState/TFDPlayerState.h"
#include "Character/TFDPlayerCharacter.h"

void UResultWidget::InitResult(FGameplayTag WinTeamTag, EGameCompleteType InCompleteType, ATFDPlayerCharacter* InOwnerCharacter)
{
    OwnerCharacter = InOwnerCharacter; // 캐릭터 저장

    if (!OwnerCharacter)
    {
        return;
    }
    // PlayerState 가져오기
    ATFDPlayerState* PS = Cast<ATFDPlayerState>(OwnerCharacter->GetPlayerState());
    if (!PS)
    {
        return;
    }

    FGameplayTag PlayerTeamTag = PS->GetTeamTag();
    FString ResultStr = TEXT("결과 없음");


    if (PlayerTeamTag.IsValid())
    {
        ResultStr = PlayerTeamTag.MatchesTagExact(WinTeamTag) ? TEXT("승리!") : TEXT("패배...");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TeamTag가 ASC에서도 유효하지 않음!"));
        return;
    }

    if (ResultText)
    {
        ResultText->SetText(FText::FromString(ResultStr));
    }

    // 승리 조건 텍스트 업데이트
    FString ConditionStr;
    switch (InCompleteType)
    {
    case EGameCompleteType::CatchAllThief:
        ConditionStr = TEXT("모든 도둑이 잡혔습니다.");
        break;
    case EGameCompleteType::TimeLimit:
        ConditionStr = TEXT("시간이 만료되었습니다.");
        break;
    case EGameCompleteType::ThiefWinByScore:
        ConditionStr = TEXT("도둑이 모든 포인트를 모았습니다.");
        break;
    default:
        ConditionStr = TEXT("");
        break;
    }

    if (ConditionText)
    {
        ConditionText->SetText(FText::FromString(ConditionStr));
    }
}


