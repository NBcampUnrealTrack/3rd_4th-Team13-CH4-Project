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
		UE_LOG(LogTemp, Warning, TEXT("AvatarActor is invalid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 서버에서만 실행
	if (!ActorInfo->AvatarActor->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server authority missing"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 바로 이동 시작
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
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, FVector(MoveRadius, MoveRadius, 0.f));

	AICon->MoveToLocation(
		RandomPoint,
		100.f,  // AcceptanceRadius
		true,  // bStopOnOverlap
		true,  // bUsePathfinding
		true,  // bCanStrafe
		true,  // bAllowPartialPath
		nullptr // OutPath
	);

	// 이동 완료 Lambda, Ability 시작 시 1번만 바인딩
	if (!bIsMoveDelegateBound)
	{
		bIsMoveDelegateBound = true;
		AICon->GetPathFollowingComponent()->OnRequestFinished.AddLambda([this](FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				// 랜덤 대기 후 다음 이동
				float WaitTime = FMath::FRandRange(MinWaitTime, MaxWaitTime);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URandomMoveAbility::DoRandomMove, WaitTime, false);
			});
	}
}
