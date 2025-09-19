// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include  "GameplayEffect.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "TFDProjectileFireAbility.generated.h"


/**
 * 
 */
UCLASS

()
class TFD_API UTFDProjectileFireAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()

public:
	UTFDProjectileFireAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
						   OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;


protected:
	// 자동시전 활성화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Ability")
	bool bIsOnAutoCast = false;
	
	// 쿨다운으로 사용할 GE 클래스 (BP에서 세팅)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Ability|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGameplayEffectClass;

protected:


};
