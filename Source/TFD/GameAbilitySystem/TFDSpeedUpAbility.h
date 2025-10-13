// TFDSpeedUpAbility.h
#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "TFDSpeedUpAbility.generated.h"

UCLASS()
class TFD_API UTFDSpeedUpAbility : public UTFDGameplayAbility
{
	GENERATED_BODY()

public:
    UTFDSpeedUpAbility();

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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Ability|Effects")
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Ability|Effects")
    float Duration;
};
