// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/SkillSlotItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "TimerManager.h"
#include "PaperSprite.h"

void USkillSlotItem::UpdateSlot(const FTFDSkillSlot& InSlot)
{
    CurrentSlot = InSlot;
    // 아이콘
    if (SkillIcon)
    {
        if (InSlot.SkillIcon)
        {
            UE_LOG(LogTemp, Error, TEXT("SkillIcon : %s"), *InSlot.SkillIcon->GetName())
            
            FSlateBrush Brush;
            Brush.SetResourceObject(InSlot.SkillIcon); // Texture가 아니라 Sprite 자체를 넣음
            SkillIcon->SetBrush(Brush);

            //SkillIcon->SetBrushFromTexture(InSlot.SkillIcon);
        }
        else
            SkillIcon->SetBrushFromTexture(nullptr);
    }

    // 사용 횟수
    if (UsageCountText)
    {
        if (InSlot.SkillTag.IsValid())
            UsageCountText->SetText(FText::Format(FText::FromString(TEXT("x{0}")),FText::AsNumber(InSlot.UsageCount)));
        else
            UsageCountText->SetText(FText::FromString(TEXT("")));
    }

    // 쿨다운
    if (InSlot.CooldownDuration > 0.f && InSlot.CooldownStartTime > 0.f)
    {
        if (GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
        {
            GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
        }

        // 1초마다 텍스트 갱신
        GetWorld()->GetTimerManager().SetTimer(
            CooldownTimerHandle,
            this,
            &USkillSlotItem::UpdateCooldownText,
            1.0f,
            true);

        UpdateCooldownText();
    }
    else
    {
        if (CooldownText)
            CooldownText->SetText(FText::GetEmpty());

        if (GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
        {
            GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
        }
    }
}

void USkillSlotItem::UpdateCooldownText()
{
    if (!CooldownText || CurrentSlot.CooldownDuration <= 0.f || CurrentSlot.CooldownStartTime <= 0.f)
        return;

    float Remaining = (CurrentSlot.CooldownStartTime + CurrentSlot.CooldownDuration) - GetWorld()->GetTimeSeconds();

    if (Remaining > 0.f)
    {
        CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(Remaining)));
    }
    else
    {
        CooldownText->SetText(FText::GetEmpty());
        GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
    }
}