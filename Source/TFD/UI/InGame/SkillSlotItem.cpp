// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/SkillSlotItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "TimerManager.h"
#include "PaperSprite.h"

void USkillSlotItem::UpdateSlot(const FTFDSkillSlot& InSlot, int32 SlotIndex)
{
    CurrentSlot = InSlot;
    // 아이콘
    if (SkillIcon)
    {
        if (InSlot.SkillIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(InSlot.SkillIcon);
            SkillIcon->SetBrush(Brush);
            SkillIcon->SetOpacity(1.0f);
        }
        else
        {
            SkillIcon->SetBrushFromTexture(nullptr);
            SkillIcon->SetOpacity(0.5f);
        }
    }

    // 사용 횟수
    if (UsageCountText)
    {
        if (InSlot.SkillTag.IsValid())
            UsageCountText->SetText(FText::Format(FText::FromString(TEXT("x{0}")),FText::AsNumber(InSlot.UsageCount)));
        else
            UsageCountText->SetText(FText::FromString(TEXT("")));
    }

    if (BindKey)
    {
        FString KeyString = FString::FromInt(SlotIndex);
        BindKey->SetText(FText::FromString(KeyString));
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
            &USkillSlotItem::UpdateCooldown,
            1.0f,
            true);

        UpdateCooldown();
    }
    else
    {
        if (CooldownText)
            CooldownText->SetText(FText::GetEmpty());

        if (CooldownBar)
            CooldownBar->SetPercent(0.f);


        if (GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
        {
            CooldownText->SetText(FText::GetEmpty());

            GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);

            FTimerHandle TempHandle;

            if (GetWorld()->GetTimerManager().IsTimerActive(TempHandle))
            {
                GetWorld()->GetTimerManager().ClearTimer(TempHandle);
            }

            if (InSlot.SkillTag.IsValid())
            { 
                SetRenderScale(FVector2D(1.2f, 1.2f));
                GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
                {
                    SetRenderScale(FVector2D(1.0f, 1.0f));
                }, 0.15f, false);
            }
        }
    }
}

void USkillSlotItem::UpdateCooldown()
{
    if (!CooldownText || CurrentSlot.CooldownDuration <= 0.f || CurrentSlot.CooldownStartTime <= 0.f)
        return;

    float Remaining = (CurrentSlot.CooldownStartTime + CurrentSlot.CooldownDuration) - GetWorld()->GetTimeSeconds();
    float Ratio = FMath::Clamp(Remaining / CurrentSlot.CooldownDuration, 0.f, 1.f);

    if (Remaining > 0.f)
    {
        CooldownText->SetText(FText::AsNumber(FMath::RoundToInt(Remaining)));
        CooldownBar->SetPercent(Ratio);
    }
    else
    {
        CooldownText->SetText(FText::GetEmpty());
        CooldownBar->SetPercent(0.0f);

        GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
    }
}