#include "UI/InGame/UW_SkillSlot.h"
#include "UI/InGame/SkillSlotItem.h"

#include "Components/HorizontalBox.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UUW_SkillSlot::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (!PC || !PC->IsLocalController()) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    UTFDSkillManagerComponent* SkillManager = PlayerPawn->FindComponentByClass<UTFDSkillManagerComponent>();
    if (!SkillManager) return;

    SkillManager->OnSkillChanged.AddDynamic(this, &UUW_SkillSlot::OnSkillChanged);
    OnSkillChanged(SkillManager->GetAllSkills());

    CachedASC = PlayerPawn->FindComponentByClass<UAbilitySystemComponent>();
    if (CachedASC)
    {
        CachedASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
            this, &UUW_SkillSlot::OnEffectAdded);

        CachedASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(
            this, &UUW_SkillSlot::OnEffectRemoved);
    }
}

void UUW_SkillSlot::OnSkillChanged(const TArray<FTFDSkillSlot>& SkillSlots)
{
    if (!SlotContainer || !SkillSlotItemClass) return;

    SlotContainer->ClearChildren();
    SlotWidgets.Empty();

    for (const FTFDSkillSlot& SlotData : SkillSlots)
    {
        USkillSlotItem* NewSlot = CreateWidget<USkillSlotItem>(this, SkillSlotItemClass);
        if (NewSlot)
        {
            NewSlot->UpdateSlot(SlotData);
            SlotContainer->AddChild(NewSlot);
            SlotWidgets.Add(NewSlot);
        }
    }
}

void UUW_SkillSlot::OnEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
    if (!CachedASC) return;

    for (USkillSlotItem* Item : SlotWidgets)
    {
        if (!Item) continue;

        const FTFDSkillSlot& SlotData = Item->GetSlotData();
        FGameplayAbilitySpec* SpecFound = CachedASC->FindAbilitySpecFromHandle(SlotData.AbilityHandle);
        if (!SpecFound || !SpecFound->Ability) continue;

        // AbilityHandle 매칭
        if (SpecFound->Handle != SlotData.AbilityHandle)
            continue;

        const FGameplayTagContainer* CooldownTagsPtr = SpecFound->Ability->GetCooldownTags();
        if (!CooldownTagsPtr || CooldownTagsPtr->Num() == 0) continue;

        FGameplayTagContainer EffectTags;
        Spec.GetAllGrantedTags(EffectTags);

        if (EffectTags.HasAny(*CooldownTagsPtr))
        {
            if (const FActiveGameplayEffect* ActiveEffect = CachedASC->GetActiveGameplayEffect(Handle))
            {
                float Duration = ActiveEffect->GetDuration();
                float Remaining = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());

                if (Duration > 0.f && Remaining > 0.f)
                {
                    Item->StartCooldown(Duration, Remaining);
                }
            }
        }
    }
}

void UUW_SkillSlot::OnEffectRemoved(const FActiveGameplayEffect& Effect)
{
    if (!CachedASC) return;

    // 제거된 이펙트의 태그 가져오기
    FGameplayTagContainer RemovedTags;
    Effect.Spec.GetAllGrantedTags(RemovedTags);

    for (USkillSlotItem* Item : SlotWidgets)
    {
        if (!Item) continue;

        const FTFDSkillSlot& SlotData = Item->GetSlotData();
        FGameplayAbilitySpec* SpecFound = CachedASC->FindAbilitySpecFromHandle(SlotData.AbilityHandle);
        if (!SpecFound || !SpecFound->Ability) continue;

        const FGameplayTagContainer* CooldownTagsPtr = SpecFound->Ability->GetCooldownTags();
        if (!CooldownTagsPtr || CooldownTagsPtr->Num() == 0) continue;

        //실제로 자기 쿨다운 태그가 제거된 경우만 StopCooldown 실행
        if (RemovedTags.HasAny(*CooldownTagsPtr))
        {
            Item->StopCooldown();
        }
    }
}
