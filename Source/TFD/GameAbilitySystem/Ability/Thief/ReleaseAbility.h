// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "ReleaseAbility.generated.h"

/* 
 */
UCLASS()
class TFD_API UReleaseAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()

public:
    UReleaseAbility();

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

    UFUNCTION()
    void OnHoldFinished();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    TSubclassOf<UGameplayEffect> ReleaseStartAnimEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    TSubclassOf<UGameplayEffect> ReleaseEndAnimEffect;
};
