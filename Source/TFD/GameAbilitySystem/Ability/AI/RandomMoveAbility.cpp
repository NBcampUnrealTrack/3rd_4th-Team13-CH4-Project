// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/AI/RandomMoveAbility.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

URandomMoveAbility::URandomMoveAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URandomMoveAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 서버에서만 실행
	if (!ActorInfo->AvatarActor->HasAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 바로 이동
	DoRandomMove();
}

void URandomMoveAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	if (ActorInfo->AvatarActor.IsValid())
	{
		ActorInfo->AvatarActor->GetWorldTimerManager().ClearTimer(TimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URandomMoveAbility::DoRandomMove()
{
	APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!Pawn) return;

	AAIController* AICon = Cast<AAIController>(Pawn->GetController());
	if (!AICon) return;

	FVector Origin = Pawn->GetActorLocation();

	// 랜덤 거리 계산
	float RandomDistance = FMath::FRandRange(0.f, MoveRadius);
	FVector RandomDirection = UKismetMathLibrary::RandomUnitVector();
	RandomDirection.Z = 0.f; // 평면 이동
	FVector RandomPoint = Origin + RandomDirection * RandomDistance;

	FNavPathSharedPtr NavPath;

	EPathFollowingRequestResult::Type Result = AICon->MoveToLocation(
		RandomPoint,
		50.f,  // AcceptanceRadius
		true,  // bStopOnOverlap
		true,  // bUsePathfinding
		true,  // bCanStrafe
		true,  // bAllowPartialPath
		nullptr // OutPath
	);

	if (Result == EPathFollowingRequestResult::RequestSuccessful)
	{
		// 이동 완료 Lambda
		AICon->GetPathFollowingComponent()->OnRequestFinished.AddLambda([this, AICon](FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				// Delegate 중복 방지
				AICon->GetPathFollowingComponent()->OnRequestFinished.Clear();

				// 랜덤 대기 후 다음 이동
				float WaitTime = FMath::FRandRange(MinWaitTime, MaxWaitTime);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URandomMoveAbility::DoRandomMove, WaitTime, false);
			});
	}
}