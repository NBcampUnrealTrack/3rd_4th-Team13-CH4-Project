// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/StopMovementAbility.h"

#include "Character/TFDCharacterBase.h"

#include "Controller/TFDPlayerController.h"
#include "AIController.h"

#include "AbilitySystemComponent.h"
#include "TFDNativeGameplayTags.h"

UStopMovementAbility::UStopMovementAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UStopMovementAbility::ActivateAbility(
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
	
	ATFDCharacterBase* StopCharacter = Cast<ATFDCharacterBase>(AvatarActor);

	if (!StopCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	
	AAIController* AICon = Cast<AAIController>(StopCharacter->GetController());

	
	if (AICon)
	{
		AICon->StopMovement();


		if (UAbilitySystemComponent* ASC = StopCharacter->GetAbilitySystemComponent())
		{
			FGameplayTagContainer CancelTags;
			CancelTags.AddTag(TAG_Ability_Neutral_RandomMove);
			ASC->CancelAbilities(&CancelTags);
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	

	StopPC = Cast<ATFDPlayerController>(StopCharacter->GetController());

	if (!StopPC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("StopMovementAbility Activate Ability"));
	StopPC->DisableInput(StopPC);
}

void UStopMovementAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (StopPC)
	{
		StopPC->EnableInput(StopPC);
		StopPC = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("StopMovementAbility End Ability"));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}