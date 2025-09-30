// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/SkillSlotItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "TimerManager.h"

void USkillSlotItem::UpdateSlot(const FTFDSkillSlot& InSlot)
{
	CurrentSlot = InSlot;
    UE_LOG(LogTemp, Error, TEXT("UpdateSlot!!!"));
	// 아이콘
	if (SkillIcon)
	{
		if (CurrentSlot.SkillIcon)
			SkillIcon->SetBrushFromTexture(CurrentSlot.SkillIcon);
		else
			SkillIcon->SetBrushFromTexture(nullptr);
	}

	// 사용횟수
	if (UsageCountText)
	{
		if (CurrentSlot.SkillTag.IsValid())
			UsageCountText->SetText(FText::AsNumber(CurrentSlot.UsageCount));
		else
			UsageCountText->SetText(FText::FromString(TEXT("")));
	}

    //쿨다운
    if (CooldownText)
    {
        CooldownText->SetText(FText::FromString(TEXT("")));
    }
}

void USkillSlotItem::StartCooldown(float Duration, float Remaining)
{
    UE_LOG(LogTemp, Error, TEXT("StartCooldown"));
    if (Remaining <= 0.f || Duration <= 0.f)
        return;

    CurrentRemainingTime = Remaining;
    UE_LOG(LogTemp, Error, TEXT("CurrentRemainingTime : %f"), CurrentRemainingTime);

    if (CooldownText)
    {
        FText Cooldown = FText::AsNumber(FMath::CeilToInt(CurrentRemainingTime));
        CooldownText->SetText(Cooldown);
    }

    // ✅ 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,
        this,
        &USkillSlotItem::UpdateCooldownUI,
        1.0f,
        true
    );
}

void USkillSlotItem::StopCooldown()
{
    UE_LOG(LogTemp, Error, TEXT("StopCooldown"));
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
    }

    CurrentRemainingTime = 0.f;

    if (CooldownText)
    {
        CooldownText->SetText(FText::FromString(TEXT("")));
    }
}

void USkillSlotItem::UpdateCooldownUI()
{
    UE_LOG(LogTemp, Error, TEXT("UpdateCooldownUI"));
    if (CurrentRemainingTime > 0.f)
    {
        CurrentRemainingTime -= 1.f;
        //UE_LOG(LogTemp, Error, TEXT("CurrentRemainingTime : %s"), *FText::AsNumber(FMath::CeilToInt(CurrentRemainingTime)).ToString());

        if (CooldownText)
        {
            UE_LOG(LogTemp, Error, TEXT("CurrentRemainingTime : %s"), *FText::AsNumber(FMath::CeilToInt(CurrentRemainingTime)).ToString());
            FText Cooldown = FText::AsNumber(FMath::CeilToInt(CurrentRemainingTime));
            CooldownText->SetText(Cooldown);

            //FString StringCooldown = LexToString(FMath::CeilToInt(CurrentRemainingTime));
            //CooldownText->SetText(FText::FromString(StringCooldown));
        }
    }
    else
    {
        StopCooldown();
    }
}