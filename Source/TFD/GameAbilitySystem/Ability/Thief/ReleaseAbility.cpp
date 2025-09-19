// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/ReleaseAbility.h"

#include "GameState/TFDGameState.h"
#include "Object/JailCell.h"

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

    UE_LOG(LogTemp, Warning, TEXT("ReleaseAbility go"));
    // 게임스테이트에서 감옥 가져와서 벽 숨기기 실행
    if (ATFDGameState* GS = ActorInfo->AvatarActor->GetWorld()->GetGameState<ATFDGameState>())
    {
        if (AJailCell* JailCell = GS->GetWorldJailCell())
        {
            JailCell->HideWallsTemporarily();
            UE_LOG(LogTemp, Warning, TEXT("Jail walls temporarily hidden via ReleaseAbility"));
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
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
