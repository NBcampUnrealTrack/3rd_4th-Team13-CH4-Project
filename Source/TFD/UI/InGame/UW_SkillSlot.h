// UW_SkillSlot.h
/*
	스킬 슬롯 위젯
*/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"
#include "UW_SkillSlot.generated.h"

class USkillSlotItem;
class UHorizontalBox;
class UAbilitySystemComponent;

UCLASS()
class TFD_API UUW_SkillSlot : public UUserWidget
{
    GENERATED_BODY()

public:
    /** SkillManager에서 전달된 슬롯 데이터 초기화 */
    UFUNCTION()
    void OnSkillChanged(const TArray<FTFDSkillSlot>& SkillSlots);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHorizontalBox> SlotContainer;

    /** 슬롯 아이템 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<USkillSlotItem> SkillSlotItemClass;

    /** 현재 슬롯 위젯들 */
    UPROPERTY()
    TArray<USkillSlotItem*> SlotWidgets;
};
