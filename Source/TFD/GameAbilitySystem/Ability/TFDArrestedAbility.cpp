#include "GameAbilitySystem/Ability/TFDArrestedAbility.h"
#include "Character/TFDCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Controller/TFDPlayerController.h"
#include "TFDNativeGameplayTags.h"

#include "Object/JailCell.h"
#include "GameState/TFDGameState.h"

UTFDArrestedAbility::UTFDArrestedAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTFDArrestedAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Invalid ActorInfo or AvatarActor"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
    if (ATFDCharacterBase* CharBase = Cast<ATFDCharacterBase>(ActorInfo->AvatarActor.Get()))
    {

        if (UAbilitySystemComponent* ASC = CharBase->GetAbilitySystemComponent())
        {
            if (ASC->HasMatchingGameplayTag(TAG_Team_Thief))
            {
                // GameState에서 감옥 가져오기
                if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
                {
                    AssignedJailCell = GS->GetWorldJailCell();
                }

                if (AssignedJailCell)
                {
                    CharBase->SetActorLocation(AssignedJailCell->GetActorLocation());
                }
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTFDArrestedAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
