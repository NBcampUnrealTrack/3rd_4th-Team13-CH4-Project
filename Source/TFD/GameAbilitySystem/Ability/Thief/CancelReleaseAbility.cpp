// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/CancelReleaseAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TFDNativeGameplayTags.h"

UCancelReleaseAbility::UCancelReleaseAbility()
{
}

void UCancelReleaseAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Log, TEXT("CancelReleaseAbility: Cancelled ability Commit fail"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("CancelReleaseAbility: Cancelled ability Commit go"));
	if (!ActorInfo || !GetAbilitySystemComponentFromActorInfo()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	// 취소할 태그 지정
	static const FGameplayTag CancelTag = TAG_Ability_Thief_Release;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Tag: %s, Active: %d"),
			*Spec.Ability->AbilityTags.ToString(), Spec.IsActive());
	}

	// 활성화된 Ability 순회
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if ((Spec.DynamicAbilityTags.HasTagExact(CancelTag) || Spec.Ability->AbilityTags.HasTagExact(CancelTag)))
		{
			if (Spec.IsActive())
			{
				UE_LOG(LogTemp, Warning, TEXT("CancelReleaseAbility: Cancelling Ability %s"), *CancelTag.ToString());
				ASC->CancelAbilityHandle(Spec.Handle);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CancelReleaseAbility: Ability found but not active yet"));
			}
		}
	}



	// 즉시 Ability 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UCancelReleaseAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
