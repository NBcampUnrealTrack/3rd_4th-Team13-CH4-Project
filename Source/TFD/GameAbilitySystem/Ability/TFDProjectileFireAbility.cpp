// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/TFDProjectileFireAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"

UTFDProjectileFireAbility::UTFDProjectileFireAbility()
{
	//어빌리티가 해당 어빌리티를 소유한 액터당 하나의 인스턴스를 생성하여 고유한 상태를 관리할 수 있도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

}


void UTFDProjectileFireAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	///	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) // 코스트/쿨타임 체크 후 적용
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}



}

void UTFDProjectileFireAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}



