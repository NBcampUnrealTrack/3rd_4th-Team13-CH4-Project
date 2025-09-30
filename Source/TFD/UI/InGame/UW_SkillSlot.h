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
    //UFUNCTION()
    //void TryBindToSkillManager();

private:
    /** 슬롯이 들어갈 컨테이너 (HorizontalBox) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHorizontalBox> SlotContainer;

    /** 슬롯 아이템 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<USkillSlotItem> SkillSlotItemClass;

    /** 현재 슬롯 위젯들 */
    UPROPERTY()
    TArray<USkillSlotItem*> SlotWidgets;

    /** ASC 캐싱 */
    UPROPERTY()
    UAbilitySystemComponent* CachedASC;

    /** 스킬 매니저 바인딩 재시도용 타이머 */
    FTimerHandle TimerHandle_RetryBinding;

    /** ASC Delegate 처리 */
    void OnEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
    void OnEffectRemoved(const FActiveGameplayEffect& Effect);
};
