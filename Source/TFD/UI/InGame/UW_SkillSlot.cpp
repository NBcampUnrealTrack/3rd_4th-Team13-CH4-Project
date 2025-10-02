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
}

void UUW_SkillSlot::OnSkillChanged(const TArray<FTFDSkillSlot>& SkillSlots)
{
    if (!SlotContainer || !SkillSlotItemClass) return;

    if (SkillSlots.IsEmpty()) return;

    // 기존 슬롯 재활용 처리
    for (int32 i = 0; i < SkillSlots.Num(); ++i)
    {
        if (SlotWidgets.IsValidIndex(i) && SlotWidgets[i])
        {
            // 기존 위젯 업데이트
            SlotWidgets[i]->UpdateSlot(SkillSlots[i], i+1);
        }
        else
        {
            // 새로운 슬롯 생성
            USkillSlotItem* NewSlot = CreateWidget<USkillSlotItem>(this, SkillSlotItemClass);
            if (NewSlot)
            {
                NewSlot->UpdateSlot(SkillSlots[i], i+1);
                SlotContainer->AddChild(NewSlot);
                SlotWidgets.Add(NewSlot);
            }
        }
    }
}