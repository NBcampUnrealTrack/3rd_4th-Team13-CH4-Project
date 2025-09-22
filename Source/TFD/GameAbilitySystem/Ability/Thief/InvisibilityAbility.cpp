// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/InvisibilityAbility.h"

#include "AbilitySystemComponent.h"

UInvisibilityAbility::UInvisibilityAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UInvisibilityAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();

	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!HideEffect)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(HideEffect, 1.f, Context);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvisibilityAbility: Handel 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("UInvisibilityAbility: Effect 실행"));
	SpecHandle.Data->SetDuration(HideDuration, true);
	ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UInvisibilityAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
