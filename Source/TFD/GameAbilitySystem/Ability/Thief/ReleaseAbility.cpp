// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/ReleaseAbility.h"

#include "GameState/TFDGameState.h"
#include "Object/JailCell.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Controller/TFDPlayerController.h"

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

    // Release 감지 Task
    UAbilityTask_WaitInputRelease* WaitRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
    if (WaitRelease)
    {
        WaitRelease->OnRelease.AddDynamic(this, &UReleaseAbility::OnInputReleasedEarly);
        WaitRelease->ReadyForActivation();
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
            UE_LOG(LogTemp, Warning, TEXT("Jail walls temporarily hidden via ReleaseAbility"));
        }
        ATFDPlayerController* GenericPC = Cast<ATFDPlayerController>(GetActorInfo().PlayerController.Get());
        if (GenericPC->IsLocalController())
        {
            GenericPC->HandleRemoveReleaseWidget();
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UReleaseAbility::OnInputReleasedEarly(float TimeHeld)
{
    // 3초 전에 키를 뗐으므로 취소
    UE_LOG(LogTemp, Warning, TEXT("ReleaseAbility: Released early after %.2f seconds, cancelling"), TimeHeld);
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
