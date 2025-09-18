// UTFDSkillManagerComponent.h
/*
	스킬 매니저 컴포넌트
*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Character/TFDPlayerDataAsset.h" // FInputActionTagMapping 사용을 위해 필요

#include "TFDSkillManagerComponent.generated.h"


//==========================
// 스킬 슬롯 구조체 정의
//==========================
USTRUCT(BlueprintType)
struct FTFDSkillSlot
{
	GENERATED_BODY()

	// 스킬의 SpecHandle (ASC에 등록된 스킬 식별자)
	UPROPERTY(VisibleAnywhere)
	FGameplayAbilitySpecHandle AbilityHandle;

	// 스킬을 식별하는 GameplayTag
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SkillTag;

	// 스킬 사용 가능 횟수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UsageCount = 1;

	FTFDSkillSlot()
		: UsageCount(1)
	{
	}
};

// 델리게이트 선언: 스킬 목록 변경 시 이벤트 전달용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChanged, const TArray<FTFDSkillSlot>&, SkillSlots);

//==========================
// 스킬 매니저 컴포넌트
//==========================
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFD_API UTFDSkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTFDSkillManagerComponent();

protected:
	virtual void BeginPlay() override;

	//==========================
	// GAS 관련
	//==========================
	// ASC 초기화 처리 (PossessedBy 또는 OnRep_PlayerState 이후 호출 필요)
	void InitializeASC();

public:
	// 스킬 추가 (자동 슬롯 배치)
	UFUNCTION(BlueprintCallable, Category = "SkillManager")
	void AddSkill(TSubclassOf<UGameplayAbility> SkillClass, FGameplayTag SkillTag, int32 InitialUsageCount = 1);

	// 스킬 제거 (태그 기준)
	UFUNCTION(BlueprintCallable, Category = "SkillManager")
	void RemoveSkill(FGameplayTag SkillTag);

	// 전체 스킬 슬롯 반환
	UFUNCTION(BlueprintCallable, Category = "SkillManager")
	const TArray<FTFDSkillSlot>& GetAllSkills() const { return SkillSlots; }

	// 특정 슬롯 반환
	UFUNCTION(BlueprintCallable, Category = "SkillManager")
	FTFDSkillSlot GetSkillAt(int32 SlotIndex) const;

	// 스킬 사용 시 호출 (사용 횟수 차감 및 제거)
	void UseSkill(FGameplayTag SkillTag);

	// 스킬 변경 이벤트 (UI 바인딩용)
	UPROPERTY(BlueprintAssignable, Category = "SkillManager")
	FOnSkillChanged OnSkillChanged;

protected:
	//==========================
	// GAS 관련
	//==========================
	// 캐릭터의 ASC 참조 (ASC는 PlayerState에 소유권이 있음)
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	// ASC 초기화 여부
	bool bASCInitialized = false;

	//==========================
	// 데이터 및 슬롯 구성
	//==========================
	// 최대 스킬 슬롯 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillManager")
	int32 MaxSkillSlotCount = 3;

	// 현재 등록된 스킬 슬롯 배열
	UPROPERTY(VisibleAnywhere, Category = "SkillManager")
	TArray<FTFDSkillSlot> SkillSlots;

	// 인풋 액션과 태그의 매핑 정보 (DataAsset에서 로딩 가능하게 설계 예정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<FInputActionTagMapping> InputActionMappings;
};