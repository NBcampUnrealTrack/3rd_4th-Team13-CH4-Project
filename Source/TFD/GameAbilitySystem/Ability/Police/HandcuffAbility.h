// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "HandcuffAbility.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UHandcuffAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHandcuffAbility();

	// Ability 실행
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	// virtual void EndAbility(
	// 	const FGameplayAbilitySpecHandle Handle,
	// 	const FGameplayAbilityActorInfo* ActorInfo,
	// 	const FGameplayAbilityActivationInfo ActivationInfo,
	// 	bool bReplicateEndAbility,
	// 	bool bWasCancelled
	// ) override;

protected:
	//Handcuff 적용 거리
	UPROPERTY(EditDefaultsOnly, Category = "Handcuff")
	float GrabRange = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Handcuff")
	FVector GrabOffset = FVector(175.f, 0, 0);

	//Handcuff 적용 시간
	UPROPERTY(EditDefaultsOnly, Category = "Handcuff")
	float CuffDuration = 1.8f;

	// 에디터에서 세팅할 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Handcuff|Anim")
	UAnimMontage* HandcuffMontage = nullptr;

	// 선택: 시작 섹션을 쓰고 싶으면
	UPROPERTY(EditDefaultsOnly, Category = "Handcuff|Anim")
	FName StartSection = NAME_None;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> ArrestedEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> HandcuffAnimEffect;

private:
	//잡힌 대상에게 상태적용
	void HandcuffToTarget(AActor* TargetActor, const FGameplayAbilityActorInfo* ActorInfo) const;

	UFUNCTION()
	void OnApplyCuffEvent(FGameplayEventData Payload); // ← AnimNotify로 쏘는 GameplayEvent 수신
	UFUNCTION()
	void OnEndCuffEvent(FGameplayEventData Payload);
};
