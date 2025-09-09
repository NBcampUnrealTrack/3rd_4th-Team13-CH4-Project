#include "GameAbilitySystem/Ability/TFDArrestedAbility.h"
#include "Character/TFDCharacter.h"
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

    ATFDCharacter* Char = Cast<ATFDCharacter>(ActorInfo->AvatarActor.Get());
    if (!Char)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] AvatarActor is not ATFDCharacter"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 1️⃣ 서버 Authority 처리 → AI Stop
    if (Char->HasAuthority())
    {
        // AI 이동 멈춤
        if (AAIController* AICon = Cast<AAIController>(Char->GetController()))
        {
            AICon->StopMovement();
            UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] AI movement stopped (Server)"));
        }

        // 캐릭터 이동 멈춤
        if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
        {
            MoveComp->StopMovementImmediately();
            MoveComp->DisableMovement();
            MoveComp->SetMovementMode(MOVE_None);
            UE_LOG(LogTemp, Warning, TEXT("[GA_Arrested] Character movement stopped (Server)"));
        }
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
        ATFDCharacter* Char = Cast<ATFDCharacter>(ActorInfo->AvatarActor.Get());
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
