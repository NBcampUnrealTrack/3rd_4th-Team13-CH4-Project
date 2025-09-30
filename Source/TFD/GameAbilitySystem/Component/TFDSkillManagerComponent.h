// UTFDSkillManagerComponent.h
/*
	스킬 매니저 컴포넌트
	- 런타임 중 스킬 추가, 제거, 사용 횟수 추적
	- 아이템 획득 이벤트를 받아 자동으로 스킬 추가
*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "TFDSkillManagerComponent.generated.h"

//==============================================
// 스킬 슬롯 구조체 정의
//==============================================
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
	int32 UsageCount;
	
	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> SkillIcon;   

	FTFDSkillSlot()
		: UsageCount(1)
		, SkillIcon(nullptr)
	{
	}
};

//==============================================
// 스킬 변경 델리게이트
// 스킬 목록 변경 시 이벤트 전달용
//==============================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChanged, const TArray<FTFDSkillSlot>&, SkillSlots);

//==============================================
// 스킬 매니저 컴포넌트
//==============================================
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFD_API UTFDSkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTFDSkillManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 리플리케이션 등록용 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 스킬 슬롯 초기화 로직 추가
	void InitializeSkillSlots();

	// Skill 슬롯 최대 개수 반환 함수
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	int32 GetMaxSlotCount() const;

	//==============================================
	// 서브시스템 이벤트 구독 관련
	//==============================================
	// 서브시스템 이벤트 구독
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void SubscribeToSkillItemEvents();

	// 서브시스템 이벤트 구독 해제
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void UnsubscribeFromSkillItemEvents();

	// 서브시스템으로부터 스킬 아이템 획득 이벤트 수신
	UFUNCTION()
	void OnSkillItemObtained(FGameplayTag SkillTag, APawn* TargetPawn);

	// 기존 방식과의 호환성을 위한 함수 (개별 아이템 이벤트 처리)
	UFUNCTION()
	void HandleSkillItemObtained(FGameplayTag SkillTag);

	//==============================================
	// 스킬 추가/제거
	//==============================================
	// 스킬 추가 (자동 슬롯 배치)
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void AddSkill(TSubclassOf<UGameplayAbility> SkillClass, FGameplayTag SkillTag, int32 InitialUsageCount = 1);

	// 스킬 추가 (자동 슬롯 배치) - 태그만 받는 버전
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void AddSkillByTag(FGameplayTag SkillTag, int32 UsageCount = 1);

	// 서버RPC - 스킬 추가 (태그 기준)
	UFUNCTION(Server, Reliable)
	void ServerAddSkillByTag(FGameplayTag SkillTag, int32 UsageCount = 1);

	// 스킬 제거 (태그 기준)
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void RemoveSkill(FGameplayTag SkillTag);

	// 전체 스킬 슬롯 반환
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	const TArray<FTFDSkillSlot>& GetAllSkills() const { return SkillSlots; }

	// 특정 슬롯 반환
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	FTFDSkillSlot GetSkillAt(int32 SlotIndex) const;

	// 슬롯 인덱스로 스킬 사용 시 호출 (사용 횟수 차감 및 제거)
	UFUNCTION(BlueprintCallable, Category = "TFD|SkillManager")
	void UseSkillAtSlot(int32 SlotIndex);

	// SkillSlots 변경 시 클라에서 호출됨 (Replication)
	UFUNCTION()
	void OnRep_SkillSlots();

	// 스킬 변경 이벤트 (UI 바인딩용)
	UPROPERTY(BlueprintAssignable, Category = "TFD|SkillManager")
	FOnSkillChanged OnSkillChanged;

	//==============================================
	// GAS 관련
	//==============================================
	// ASC 참조 설정 (OnRep_PlayerState 이후 등에서 호출 필요)
	void SetupASC();

	// ASC 초기화 여부 확인
	bool IsASCSetup() const;

protected:
	//==============================================
	// GAS 관련
	//==============================================
	// 캐릭터의 ASC 참조 (ASC는 PlayerState에 소유권이 있음)
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	// ASC 초기화 여부
	bool bASCSetup = false;

	//==============================================
	// 슬롯 데이터 및 설정
	//==============================================
	// 슬롯 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|SkillManager")
	int32 MaxSkillSlotCount = 3;

	// 스킬 슬롯 배열
	UPROPERTY(ReplicatedUsing = OnRep_SkillSlots, VisibleAnywhere, Category = "TFD|SkillManager")
	TArray<FTFDSkillSlot> SkillSlots;

	//==================================================
	// 스킬 태그 -> 클래스 매핑 (에디터에서 설정)
	//==================================================
	UPROPERTY(EditDefaultsOnly, Category = "TFD|SkillManager", meta = (TitleProperty = "Key"))
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> SkillTagToClassMap;

	//추가: 태그 → 아이콘 매핑
	UPROPERTY(EditDefaultsOnly, Category = "TFD|SkillManager")
	TMap<FGameplayTag, TObjectPtr<UTexture2D>> SkillTagToIconMap;

	//==============================================
	// 서브시스템 구독 상태 추적
	//==============================================
	bool bIsSubscribedToSubsystem = false;

private:
	//==============================================
	// 스킬별 처리 함수들
	//==============================================
	//void HandleInvisibilitySkill();
	//void HandleFireballSkill();
};