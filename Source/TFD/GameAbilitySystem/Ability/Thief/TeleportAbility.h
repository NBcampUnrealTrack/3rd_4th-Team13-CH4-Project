// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "TeleportAbility.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UTeleportAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()
public:
    UTeleportAbility();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Setting")
    float TeleportDistance = 1000.f; // 전방 탐지 거리

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Setting")
    float BackOffset = 50.f;         // 맞은 위치에서 뒤로 이동할 거리
};
