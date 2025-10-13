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
#include "Styling/SlateBrush.h"
#include "UI/UIResourceAsset.h"
#include "GameInstance/TFDGameInstance.h"
#include "NativeGameplayTags.h"

void USkillSlotItem::UpdateSlot(const FTFDSkillSlot& InSlot, int32 SlotIndex)
{
    CurrentSlot = InSlot;

    if (SkillIcon)
    {
        //아이콘 설정
        if (!GetWorld()) return;
        
        auto* GI = Cast<UTFDGameInstance>(GetWorld()->GetGameInstance());
        if (!GI) return;

        EUIIconType IconType = EUIIconType::None;

        if (InSlot.SkillTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Ability.Thief.Invisibility")))
        {
            IconType = EUIIconType::Invisibility;
        }
        else if (InSlot.SkillTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Ability.Thief.Teleport")))
        {
            IconType = EUIIconType::Teleport;
        }
        else if (InSlot.SkillTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Ability.FireProjectile")))
        {
            IconType = EUIIconType::ThrowSlowItem;
        }
        else if (InSlot.SkillTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Ability.SpeedUp")))
        {
            IconType = EUIIconType::SpeedUp;
        }

        if(IconType != EUIIconType::None)
        {
            const TSoftObjectPtr<UPaperSprite>* Found = GI->LoadedUIResource->IconMap.Find(IconType);
            if (!Found) return;

            UPaperSprite* Sprite = Found->LoadSynchronous();
            if (!Sprite) return;

            const FVector2D Size = SkillIcon->GetDesiredSize();
            FSlateBrush Brush = UUIResourceAsset::MakeBrushFromSprite(Sprite, Size.X, Size.Y);
            SkillIcon->SetBrush(Brush);
            SkillIcon->SetOpacity(1.f);
        }
        else
        {
            FSlateBrush EmptyBrush;
            EmptyBrush.SetResourceObject(nullptr);
            SkillIcon->SetBrush(EmptyBrush);
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

            FTimerHandle TempHandle; //쿨다운 완료됐을때 순간적인 크기변경으로 강조하기

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