// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TFDDashAbility.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UTFDDashAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UTFDDashAbility();

protected:
	// 대시 설정값들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDistance = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDuration = 0.2f;

	// 실제 스킬 로직
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UFUNCTION()
	void OnDashComplete();

	FTimerHandle DashTimerHandle;

	// 이전 속도(복원용, 필요하면 사용)
	float PrevMaxWalkSpeed = 0.f;
	
	UPROPERTY()
	ACharacter* DashingCharacter;
	
};
