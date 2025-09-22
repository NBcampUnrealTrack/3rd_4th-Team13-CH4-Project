// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "InvisibilityAbility.generated.h"

/*
 * 은신 Ability
 * - 단순히 HideEffect를 적용
 * - HideDuration으로 지속시간 지정 가능
 * - 실제 Mesh Visibility 등은 GameplayCue에서 처리
 */
UCLASS()
class TFD_API UInvisibilityAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()
public:
    UInvisibilityAbility();

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

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    TSubclassOf<UGameplayEffect> HideEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    float HideDuration;
};
