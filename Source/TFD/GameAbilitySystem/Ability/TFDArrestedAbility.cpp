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
    
    if(ATFDCharacterBase * CharBase = Cast<ATFDCharacterBase>(ActorInfo->AvatarActor.Get()))
    {
        if (UAbilitySystemComponent* ASC = CharBase->GetAbilitySystemComponent())
        {
            if (ASC->HasMatchingGameplayTag(TAG_Team_Thief))
            {
                if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(CharBase->GetController()))
                {
                    PC->SetMovemnetWalking(false);
                    PC->HandleRemoveReleaseWidget();
                }
            }
            else if (ASC->HasMatchingGameplayTag(TAG_Team_Neutral))
            {
                // AI 처리
                if (AAIController* AICon = Cast<AAIController>(CharBase->GetController()))
                {
                    AICon->StopMovement();
                }

                FGameplayTagContainer CancelTags;
                CancelTags.AddTag(TAG_Ability_Neutral_RandomMove);
                ASC->CancelAbilities(&CancelTags);
            }
        }
    }

    //현재 경찰의 패널티를 주는게 없어서 EndAbility 실행. 필요하다면 타이머로 특정 시간 후 실행되도록 변경
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTFDArrestedAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    /* 캐릭터 움직임 다시 풀기. 현재는 필요없으니 주석 처리.
    if (ATFDCharacterBase* CharBase = Cast<ATFDCharacterBase>(ActorInfo->AvatarActor.Get()))
    {
        if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(CharBase->GetController()))
        {
            PC->SetMovemnetWalking(true);
        }
    }
    */

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

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
