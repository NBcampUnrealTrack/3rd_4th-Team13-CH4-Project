// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/ReleaseWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "TFDNativeGameplayTags.h"

void UReleaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (AActor* OwningActor = GetOwningPlayerPawn())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
        {
            FGameplayTag ReleaseActiveTag = TAG_Ability_Thief_Release_Apply;
            ASC->GenericGameplayEventCallbacks.FindOrAdd(ReleaseActiveTag).AddUObject(this, &UReleaseWidget::HandleReleaseAbilityActivated);

            FGameplayTag ReleaseCancelTag = TAG_Ability_Thief_Release_End;
            ASC->GenericGameplayEventCallbacks.FindOrAdd(ReleaseCancelTag).AddUObject(this, &UReleaseWidget::HandleReleaseAbilityCanceled);
        }
    }

    if (ReleaseProgressBar)
    {
        ReleaseProgressBar->SetPercent(0.f);
    }

}

void UReleaseWidget::UpdateReleaseProgress()
{
    if (!ReleaseProgressBar) return;

    ElapsedTime += 0.1f; // 타이머 주기만큼 증가

    float Percent = FMath::Clamp(ElapsedTime / FillDuration, 0.f, 1.f);
    ReleaseProgressBar->SetPercent(Percent);

    // 다 차면 타이머 정지
    if (Percent >= 1.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(FillTimerHandle);
    }
}

void UReleaseWidget::HandleReleaseAbilityActivated(const FGameplayEventData* Payload)
{
    UE_LOG(LogTemp, Warning, TEXT("Release Ability 시작 UI 알림 받음!"));

    if (ReleaseProgressBar)
    {
        ReleaseProgressBar->SetPercent(0.f);
    }
    ElapsedTime = 0.f;

    // 경과 시간 초기화
    UpdateReleaseProgress(); // 즉시 0으로 반영
    GetWorld()->GetTimerManager().SetTimer(
        FillTimerHandle,
        this,
        &UReleaseWidget::UpdateReleaseProgress,
        0.1f,   // 0.1초마다 호출
        true
    );
}

void UReleaseWidget::HandleReleaseAbilityCanceled(const FGameplayEventData* Payload)
{
    UE_LOG(LogTemp, Warning, TEXT("Release Ability 취소 UI 알림 받음!"));
    GetWorld()->GetTimerManager().ClearTimer(FillTimerHandle);

    // 값 초기화
    ElapsedTime = 0.f;
    if (ReleaseProgressBar)
    {
        ReleaseProgressBar->SetPercent(0.f);
    }
}

