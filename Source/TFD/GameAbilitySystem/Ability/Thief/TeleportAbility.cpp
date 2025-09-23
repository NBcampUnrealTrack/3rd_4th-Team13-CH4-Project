// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Thief/TeleportAbility.h"

UTeleportAbility::UTeleportAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTeleportAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();

	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector Start = AvatarActor->GetActorLocation();
	FVector Forward = AvatarActor->GetActorForwardVector();

	FVector End = Start + Forward * TeleportDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AvatarActor);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	FVector TeleportLocation = End;

	if (bHit)
	{
		TeleportLocation = HitResult.Location - Forward * BackOffset;

		FHitResult GroundHit;
		FVector GroundStart = Start;
		GroundStart.Z = 0.5f;
		End.Z = 0.5f;

		if (GetWorld()->LineTraceSingleByChannel(GroundHit, GroundStart, End, ECC_Visibility))
		{
			TeleportLocation = GroundHit.Location - Forward * BackOffset;
			TeleportLocation.Z = 90.5f;
		}
	}

    // 순간 이동
    AvatarActor->SetActorLocation(TeleportLocation);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTeleportAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
