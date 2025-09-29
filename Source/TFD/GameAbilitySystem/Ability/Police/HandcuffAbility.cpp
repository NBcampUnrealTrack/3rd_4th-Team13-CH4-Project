#include "GameAbilitySystem/Ability/Police/HandcuffAbility.h"
#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/TFDCharacterBase.h"
#include "GameMode/TFDGameMode.h"
#include "TFDNativeGameplayTags.h"
//디버그용
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"


UHandcuffAbility::UHandcuffAbility()
{
	AbilityTags.AddTag(TAG_Ability_Cop_Handcuff);
	
	//필요하면 쿨다운, 코스트, 차단 조건 추가

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 입력 즉시 반응 + 서버 검증
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy  = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void UHandcuffAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// 코스트/쿨다운 등 조건 확인 (현재는 없음)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Handcuff Ability Commit Fail"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!HandcuffAnimEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandcuffAnimEffect is invalid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (HasAuthority(&ActivationInfo) == false)
	{
		return;
	}

	bCatchThief = true;

	UAbilitySystemComponent* ASC = GetCurrentActorInfo()->AbilitySystemComponent.Get();

	if (!ASC) return;
	
	FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(
		HandcuffAnimEffect, GetAbilityLevel(), ASC->MakeEffectContext()
	);
	if (EffectSpec.IsValid() == false)
		return;
	
	// 자신에게 적용
	FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	if (EffectHandle.IsValid() && EffectHandle.WasSuccessfullyApplied())
	{
		ApplyStopEffect(ActorInfo->AvatarActor.Get(), CuffDuration);

		UAbilityTask_WaitGameplayEvent* WaitEvent =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Ability_Cop_HandCuff_Apply, nullptr, true, true);
		WaitEvent->EventReceived.AddDynamic(this, &UHandcuffAbility::OnApplyCuffEvent);
		WaitEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitEventAnimEnd =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Ability_Cop_HandCuff_End,
				nullptr, true, true);
		WaitEventAnimEnd->EventReceived.AddDynamic(this, &UHandcuffAbility::OnEndCuffEvent);
		WaitEventAnimEnd->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandcuffAnimEffect apply fail"));
	}
}


void UHandcuffAbility::HandcuffToTarget(AActor* TargetActor, const FGameplayAbilityActorInfo* ActorInfo)
{
	// 잡힌 대상에게 상태 부여
	if (!GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Handcuff To Target"));

	if (!ActorInfo->AbilitySystemComponent.Get() || !ArrestedEffect)
	{
		return;
	}

	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(
		ArrestedEffect, GetAbilityLevel(), Context);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(TargetActor);
	if (!CB)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATFDCharacterBase null"));
		return;
	}

	FActiveGameplayEffectHandle Handle =
		ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
			*SpecHandle.Data.Get(),
			CB->GetAbilitySystemComponent()
		);

	if (!Handle.IsValid())
	{
		return;
	}

	// 도둑인 경우 GameMode 호출
	if (UWorld* World = GetWorld())
	{
		if (ATFDGameMode* GM = World->GetAuthGameMode<ATFDGameMode>())
		{
			if (APawn* TargetPawn = Cast<APawn>(TargetActor))
			{
				bCatchThief = GM->OnCatchThief(TargetPawn);

				UE_LOG(LogTemp, Warning, TEXT("OnCatchThief called for %s | Result: %s"),
					*TargetPawn->GetName(),
					bCatchThief ? TEXT("true") : TEXT("false"));
			}
		}
	}

	/*
	UE_LOG(LogTemp, Warning, TEXT("Handcuff To Target"));

	if (ActorInfo->AbilitySystemComponent.Get() && ArrestedEffect)
	{
		FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(
			ArrestedEffect, GetAbilityLevel(), Context);

		if (SpecHandle.IsValid())
		{
			ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(TargetActor);
			if(!CB) 
			{
				UE_LOG(LogTemp, Warning, TEXT("ATFDCharacterBase null"));
				return;
			}
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), CB->GetAbilitySystemComponent());
		}
	}
	*/
}

void UHandcuffAbility::OnApplyCuffEvent(FGameplayEventData Payload)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector Origin = GetAvatarActorFromActorInfo()->GetActorLocation() + GrabOffset;
	TArray<FOverlapResult> Overlaps;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRange);
	World->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		Sphere
	);

	for (FOverlapResult Result : Overlaps)
	{
		AActor* TargetActor = Result.GetActor();
		if (!TargetActor || TargetActor == GetAvatarActorFromActorInfo()) continue;

		ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(TargetActor);
		if (!CB) continue;

		UAbilitySystemComponent* TargetASC = CB->GetAbilitySystemComponent();
		if (!TargetASC) continue;

		// 조건: 경찰 제외, 이미 Arrested 상태 아닌 경우
		if (!TargetASC->HasMatchingGameplayTag(TAG_Team_Cop) &&
			!TargetASC->HasMatchingGameplayTag(TAG_Character_State_Arrested))
		{
			if (GetCurrentActorInfo()->IsNetAuthority())
			{
				HandcuffToTarget(TargetActor, GetCurrentActorInfo());
			}
		}
	}
}

void UHandcuffAbility::OnEndCuffEvent(FGameplayEventData Payload)
{
	if (bCatchThief)
	{
		UE_LOG(LogTemp, Warning, TEXT("도둑 잡았거나 AI 안잡음"));
	}
	else
	{
		ApplyDemeritEffect(DemeritTime);
		UE_LOG(LogTemp, Warning, TEXT("AI 잡음"));
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHandcuffAbility::ApplyDemeritEffect(float Duration)
{
	if (!DemeritEffect) return;

	// 대상의 AbilitySystemComponent 가져오기
	ATFDCharacterBase* TargetCH = Cast<ATFDCharacterBase>(CurrentActorInfo->AvatarActor.Get());

	if (!TargetCH) return;

	UAbilitySystemComponent* ASC = TargetCH->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DemeritEffect, 1.0f, Context);
	if (!SpecHandle.IsValid()) return;
	UE_LOG(LogTemp, Warning, TEXT("DemeritEffect Excution"));
	SpecHandle.Data->SetDuration(Duration, true);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}
