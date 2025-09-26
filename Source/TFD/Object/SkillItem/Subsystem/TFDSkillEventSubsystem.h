// TFDSkillEventSubsystem.h
/*
	스킬 이벤트 서브시스템
	모든 스킬 아이템과 스킬 매니저 간의 이벤트를 중계하는 역할
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "TFDSkillEventSubsystem.generated.h"

// // 스킬 아이템 획득 이벤트 델리게이트 (모든 스킬 매니저가 구독)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillItemObtainedSubsystem, FGameplayTag, SkillTag, APawn*, TargetPawn);

UCLASS()
class TFD_API UTFDSkillEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	// 서브시스템 초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 서브시스템 종료
	virtual void Deinitialize() override;

	// 플레이어별 스킬 아이템 획득 이벤트 (타겟 플레이어 정보 포함)
	UPROPERTY(BlueprintAssignable, Category = "TFD|Skill Events")
	FOnSkillItemObtainedSubsystem OnSkillItemObtained;

	// 특정 플레이어에게 스킬 아이템 획득을 브로드캐스트하는 함수
	UFUNCTION(BlueprintCallable, Category = "TFD|Skill Events")
	void BroadcastSkillItemObtained(FGameplayTag SkillTag, APawn* TargetPawn);

	// 직접 타겟 플레이어의 스킬 매니저에 스킬 추가
	UFUNCTION(BlueprintCallable, Category = "TFD|Skill Events")
	bool AddSkillDirectlyToPlayer(FGameplayTag SkillTag, APawn* TargetPawn, int32 UsageCount = 1);

	// 서브시스템 인스턴스를 쉽게 가져오는 정적 함수
	UFUNCTION(BlueprintCallable, Category = "TFD|Skill Events", meta = (WorldContext = "WorldContextObject", ExpandBoolAsExecs = "ReturnValue"))
	static UTFDSkillEventSubsystem* GetSkillEventSubsystem(const UObject* WorldContextObject, bool& bIsValid);

	// 간단한 버전 (bool 반환 없음)
	UFUNCTION(BlueprintCallable, Category = "TFD|Skill Events", meta = (WorldContext = "WorldContextObject"))
	static UTFDSkillEventSubsystem* GetSkillEventSubsystemSimple(const UObject* WorldContextObject);
};
