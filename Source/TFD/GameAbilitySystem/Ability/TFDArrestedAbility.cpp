#include "GameAbilitySystem/Ability/TFDArrestedAbility.h"
#include "Character/TFDPlayerCharacter.h"
#include "Character/TFDAICharacter.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Controller/TFDPlayerController.h"
#include "TFDNativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Invalid ActorInfo or AvatarActor"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
    if (ATFDPlayerCharacter* PlayerChar = Cast<ATFDPlayerCharacter>(ActorInfo->AvatarActor.Get()))
    {
        if (UAbilitySystemComponent* ASC = PlayerChar->GetAbilitySystemComponent())
        {
            // TAG_Team_Thief 태그를 가진 경우만 실행
            if (ASC->HasMatchingGameplayTag(TAG_Team_Thief))
            {
                // 캐릭터 이동 멈춤
                if (UCharacterMovementComponent* MoveComp = PlayerChar->GetCharacterMovement())
                {
                    MoveComp->StopMovementImmediately();
                    MoveComp->DisableMovement();
                    MoveComp->SetMovementMode(MOVE_None);

                    UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Thief character movement stopped (Server)"));
                }
            }
        }
    }
    else if (ATFDAICharacter* AIChar = Cast<ATFDAICharacter>(ActorInfo->AvatarActor.Get()))
    {
        // AI 이동 멈춤
        if (AAIController* AICon = Cast<AAIController>(AIChar->GetController()))
        {
            AICon->StopMovement();
            UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] AI movement stopped (Server)"));
        }

        if (UAbilitySystemComponent* ASC = AIChar->GetAbilitySystemComponent())
        {
            FGameplayTagContainer CancelTags;
            CancelTags.AddTag(TAG_Ability_Neutral_RandomMove);

            ASC->CancelAbilities(&CancelTags);
            UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] AI RandomMove ability canceled"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Unknown AvatarActor type"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UTFDArrestedAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        ATFDPlayerCharacter* Char = Cast<ATFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
        if (Char && ActorInfo->IsLocallyControlled())
        {
            if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(Char->GetController()))
            {
                Char->EnableInput(PC);
                UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Player input enabled (Client)"));
            }
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
