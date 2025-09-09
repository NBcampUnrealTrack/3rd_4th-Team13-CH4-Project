// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "RandomMoveAbility.generated.h"

/**
 *
 */
UCLASS()
class TFD_API URandomMoveAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()

public:
	URandomMoveAbility();

	// Ability 실행
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

	void DoRandomMove();
private:

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MinWaitTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxWaitTime = 3.f;

	// 델리게이트 바인딩 여부 체크
	bool bIsMoveDelegateBound = false;
};
