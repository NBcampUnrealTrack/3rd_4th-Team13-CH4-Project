// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/ReleaseAbility.h"

#include "GameState/TFDGameState.h"
#include "Object/JailCell.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Controller/TFDPlayerController.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

UReleaseAbility::UReleaseAbility()
{
}

void UReleaseAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 3초 Delay Task
    UAbilityTask_WaitDelay* WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 3.0f);
    if (WaitDelay)
    {
        WaitDelay->OnFinish.AddDynamic(this, &UReleaseAbility::OnHoldFinished);
        WaitDelay->ReadyForActivation();
    }


}

void UReleaseAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
    UE_LOG(LogTemp, Warning, TEXT("ReleaseAbility end"));

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UReleaseAbility::OnHoldFinished()
{
    if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
    {
        if (AJailCell* JailCell = GS->GetWorldJailCell())
        {
            JailCell->HideWallsTemporarily();
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

