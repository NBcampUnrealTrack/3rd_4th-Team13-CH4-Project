#include "GameAbilitySystem/Ability/Police/HandcuffAbility.h"
#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/TFDCharacterBase.h"
#include "GameMode/TFDGameMode.h"

//디버그용
#include "DrawDebugHelpers.h"


UHandcuffAbility::UHandcuffAbility()
{
	AbilityTags.AddTag(TAG_Ability_Cop_Handcuff);
	
	//필요하면 쿨다운, 코스트, 차단 조건 추가

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated; // 로컬 입력 → 서버 실행
}

void UHandcuffAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	// 코스트/쿨다운 등 조건 확인 (현재는 없음)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Handcuff Ability Commit Fail"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//몽타주가 세팅되어 있으면 재생 + 콜백 대기
	if (HandcuffMontage && ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		UAbilityTask_PlayMontageAndWait* PlayTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,  //태스크를 소유하는 어빌리티 객체
				NAME_None, //같은 어빌리티 내에서 태스크를 구분하거나 디버깅할 때 사용.(특별한 용도 없으면 NAME_None)
				HandcuffMontage, //재생할 UAnimMontage 에셋
				1.f, //재생 속도 배수
				StartSection, //재생을 어느 섹션부터 시작할지 (NAME_None 이면 섹션 점프 없이 처음부터)
				true, //어빌리티가 종료될 때 몽타주도 함께 멈출지 여부
				1.f //몽타주에서 발생하는 루트 모션의 이동량 스케일
			);

		PlayTask->OnCompleted.AddDynamic(this, &UHandcuffAbility::OnHandcuffMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &UHandcuffAbility::OnHandcuffMontageInterrupted);
		PlayTask->OnCancelled.AddDynamic(this, &UHandcuffAbility::OnHandcuffMontageCancelled);
		PlayTask->ReadyForActivation();

		// ◆ AnimNotify에서 GameplayEvent를 쏘면 여기서 수신
		//    예: AnimNotify_Blueprint에서 "Event.Ability.Handcuff.Apply" 태그로 이벤트 발사

		const FGameplayTag ApplyTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Cop.Handcuff"));

		if (ApplyTag.IsValid())
		{
			UAbilityTask_WaitGameplayEvent* WaitEvent =
				UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ApplyTag, nullptr, true, true);
			WaitEvent->EventReceived.AddDynamic(this, &UHandcuffAbility::OnApplyCuffEvent);
			WaitEvent->ReadyForActivation();
		}

		return; // 완료/취소/인터럽트 콜백에서 종료
	}

	AActor* Target = FindTarget(ActorInfo);
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Handcuff Ability : FindTarget null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	HandcuffToTarget(Target, ActorInfo);

	// 예시: 즉시 종료(모션/Montage를 쓸 거면 Notify에서 종료)
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UHandcuffAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AActor* UHandcuffAbility::FindTarget(const FGameplayAbilityActorInfo* ActorInfo) const
{

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid()) return nullptr;
	
	ACharacter* Self = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Self) return nullptr;
	
	// 아주 단순한 전방 라인 트레이스: 필요 시 콜리전 채널/필터링 확장
	FVector Start = Self->GetActorLocation();
	FVector End = Start + Self->GetActorForwardVector() * GrabRange;
			FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HandcuffTrace), false, Self);
	bool bHit = Self->GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Pawn, Params);

	//디버그용으로 로컬에서만 보이면됨
	if (IsLocallyControlled())
	{
		FColor LineColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(
			Self->GetWorld(),
			Start,
			End,
			LineColor,
			false,   // bPersistentLines (true면 영구 표시)
			3.0f,
			0,
			2.0f
		);

		if (bHit)
		{
			DrawDebugSphere(
				Self->GetWorld(),
				Hit.ImpactPoint,
				10.0f, // 반지름
				12,    // 세그먼트
				FColor::Yellow,
				false,
				2.0f
			);
		}	
	}

	return bHit ? Hit.GetActor() : nullptr;
	
}

void UHandcuffAbility::HandcuffToTarget(AActor* TargetActor, const FGameplayAbilityActorInfo* ActorInfo)
{
	//잡힌 대상에게 상태부여
	if (GetCurrentActorInfo()->IsNetAuthority())
	{
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
				if (!CB)
				{
					UE_LOG(LogTemp, Warning, TEXT("ATFDCharacterBase null"));
					return;
				}

				if (!CB->GetAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Team_Cop))
				{
					// 이미 Arrested 상태면 스킵
					if (!CB->GetAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Character_State_Arrested))
					{
						// 경찰이 아니고, Arrested 태그도 없는 경우 이펙트 적용
						ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
							*SpecHandle.Data.Get(),
							CB->GetAbilitySystemComponent()
						);
						if (CB->GetAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Team_Thief))
						{
							// 도둑인 경우 GameMode 호출
							if (UWorld* World = GetWorld())
							{
								if (ATFDGameMode* GM = World->GetAuthGameMode<ATFDGameMode>())
								{
									if (APawn* TargetPawn = Cast<APawn>(TargetActor))
									{
										GM->OnCatchThief(TargetPawn);
										UE_LOG(LogTemp, Warning, TEXT("OnCatchThief called for %s"), *TargetPawn->GetName());
									}
								}
							}
						}
						
					}
				}
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

void UHandcuffAbility::OnHandcuffMontageCompleted()
{
	K2_EndAbility();
}

void UHandcuffAbility::OnHandcuffMontageInterrupted()
{
	K2_EndAbility();
}

void UHandcuffAbility::OnHandcuffMontageCancelled()
{
	K2_EndAbility();
}

void UHandcuffAbility::OnApplyCuffEvent(FGameplayEventData Payload)
{
	
	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (AActor* Target = FindTarget(Info))
		{
			HandcuffToTarget(Target, Info);
		}
	}
}
