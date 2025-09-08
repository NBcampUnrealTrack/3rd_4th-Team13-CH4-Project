// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/TFDDashAbility.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

UTFDDashAbility::UTFDDashAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	DashingCharacter = nullptr;
}

void UTFDDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	// Cost와 Cooldown 체크 (현재는 둘 다 없음)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dash] CommitAbility failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("[Dash] ActivateAbility called"));

	// 캐릭터 가져오기

	DashingCharacter = ActorInfo && ActorInfo->AvatarActor.IsValid()
					   ? Cast<ACharacter>(ActorInfo->AvatarActor.Get())
					   : nullptr;

	if (!DashingCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dash] No AvatarActor / not a character"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
		
	}

	UCharacterMovementComponent* MoveComp = DashingCharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dash] No CharacterMovementComponent"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// 방향/속도 계산
	FVector Dir = DashingCharacter->GetActorForwardVector().GetSafeNormal();
	if (Dir.IsNearlyZero())
	{
		Dir = FVector::ForwardVector;
	}

	// 대시 속도 = 거리 / 시간
	FVector LaunchVel = Dir * (DashDistance / FMath::Max(0.001f, DashDuration));

	// 필요하면 기존 속도나 MaxWalkSpeed 저장
	PrevMaxWalkSpeed = MoveComp->MaxWalkSpeed;

	// 속도 초기화 후 Launch
	MoveComp->StopMovementImmediately();
	DashingCharacter->LaunchCharacter(LaunchVel, true, true);

	// DashDuration 후 OnDashComplete 호출
	UWorld* World = DashingCharacter->GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DashTimerHandle, this, &UTFDDashAbility::OnDashComplete, DashDuration, false);
	}

	UE_LOG(LogTemp, Log, TEXT("[Dash] Launched with vel %s, duration %f"), *LaunchVel.ToString(), DashDuration);
	
}

void UTFDDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                 bool bWasCancelled)
{
	// 타이머 정리
	if (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
	{
		if (UWorld* World = CurrentActorInfo->AvatarActor.Get()->GetWorld())
		{
			World->GetTimerManager().ClearTimer(DashTimerHandle);
		}
	}

	DashingCharacter = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[Dash] EndAbility"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTFDDashAbility::OnDashComplete()
{
	UE_LOG(LogTemp, Log, TEXT("[Dash] OnDashComplete called"));


	if (DashingCharacter)
	{
		if (UCharacterMovementComponent* MoveComp = DashingCharacter->GetCharacterMovement())
		{
			// 대시 종료 시 속도 정리
			MoveComp->StopMovementImmediately();
			MoveComp->MaxWalkSpeed = PrevMaxWalkSpeed; // 필요시 복원
		}
	}

	// 안전하게 EndAbility 호출
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
