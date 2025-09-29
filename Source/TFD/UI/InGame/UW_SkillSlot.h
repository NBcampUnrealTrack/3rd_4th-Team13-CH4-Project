// UW_SkillSlot.h
/*
	스킬 슬롯 위젯
*/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/TimerHandle.h"
#include "UW_SkillSlot.generated.h"

class UTextBlock;

UCLASS()
class TFD_API UUW_SkillSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//// 특정 슬롯 인덱스에 대한 업데이트를 처리하는 함수
	//UFUNCTION(BlueprintCallable, Category = "SkillSlot")
	//void UpdateSkillSlot(int32 SlotIndex, const FTFDSkillSlot& SkillSlot);

	// 스킬 슬롯에 관련된 정보들 (예: 스킬 아이콘, 사용횟수 등)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Skillinfo;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//TObjectPtr<UTextBlock> UsageCountText;

	//UPROPERTY(meta = (BindWidgetOptional))
	//class UImage* SkillIcon;

	// 스킬 변경 이벤트를 처리하는 함수 (델리게이트)
	UFUNCTION()
	void OnSkillChanged(const TArray<FTFDSkillSlot>& SkillSlots);

protected:
	// 위젯이 생성될 때 호출되는 함수
	virtual void NativeConstruct() override;

	// 스킬 매니저 바인딩을 재시도하는 함수
	UFUNCTION()
	void TryBindToSkillManager();

private:
	FTFDSkillSlot CurrentSkillSlot;

	// 바인딩 재시도용 타이머 핸들
	FTimerHandle TimerHandle_RetryBinding;
};