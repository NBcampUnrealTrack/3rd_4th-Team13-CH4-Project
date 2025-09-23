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
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	// 1. 쿨다운 체크
	if (ASC->HasMatchingGameplayTag(CooldownTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("쿨다운 중!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//1-2쿨타임적용
	if (CooldownGEClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("쿨다운 시작!"));
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGEClass, GetAbilityLevel());
		SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	// 2. Cost 적용
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTFDProjectileFireAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("FFF 엔드2"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


FGameplayTagContainer UTFDProjectileFireAbility::GetCoolDownTags() const
{
	FGameplayTagContainer CooldownTags;
	CooldownTags.AddTag(FGameplayTag::RequestGameplayTag("Ability.FireProjectile.CoolDown"));
	return CooldownTags;
}

void UTFDProjectileFireAbility::OnProjectileFired()
{
	UE_LOG(LogTemp, Warning, TEXT("FFF 엔드!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
