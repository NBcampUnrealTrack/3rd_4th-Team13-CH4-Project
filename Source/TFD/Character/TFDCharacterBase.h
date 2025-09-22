#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Logging/LogMacros.h"
#include "TFDPlayerDataAsset.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h" // 스킬 매니저 컴포넌트
#include "TFDCharacterBase.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class TFD_API ATFDCharacterBase
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ATFDCharacterBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void NotifyControllerChanged() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	스킬 매니저 컴포넌트 관련

	- BeginPlay
	테스트 중 확인 내용은 다음과 같음.
	BeginPlay에서 이미 SkillManagerComponent가 ASC 갖고오기를 성공적으로 함.
	BeginPlay 시점에 PlayerState가 유효해서 가능했음.

	- PossessedBy, OnRep_PlayerState
	이게 항상 보장되는가?
	네트워크 환경이나 리플리케이션 타이밍에 따라
	BeginPlay 시점에 PlayerState가 없거나 완전하지 않은 경우가 있을 수 있음.
	따라서 PossessedBy와 OnRep_PlayerState에서도
	ASC 초기화를 시도하는 안전장치로 두는 경우가 많다고 하여,
	예외 상황을 대비.

	- GetSkillManagerComponent
	스킬 매니저 컴포넌트 접근용 Getter

	- ServerUseSkillAtSlot
	서버RPC로 스킬 사용 요청
*/
	UFUNCTION(BlueprintCallable, Category = "Skill")
	UTFDSkillManagerComponent* GetSkillManagerComponent() const { return SkillManagerComponent; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseSkillAtSlot(int32 SlotIndex);

protected:
	// GAS 기본 생성자
	void BaseSetting();
	void SetDAPlayerStat();
	void OnSpeedAttributeChanged(const FOnAttributeChangeData& Data);
	void OnGoldAttributeChanged(const FOnAttributeChangeData& Data);

protected:
	// GAS 핵심 컴포넌트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UTFDAttributeSet* AttributeSet;

public:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSoftObjectPtr<UTFDPlayerDataAsset> CharacterData;

	// 스킬 매니저 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	UTFDSkillManagerComponent* SkillManagerComponent;
};

