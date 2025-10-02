// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayCue/GameplayCue_Release.h"

#include "AnimInstance/TFDAnimInstanceBase.h"
#include "Character/TFDCharacterBase.h"
#include "Character/TFDAICharacter.h"

void AGameplayCue_Release::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	ACharacter* Character = Cast<ACharacter>(MyTarget);
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;

	UTFDAnimInstanceBase* AnimInstance = Cast<UTFDAnimInstanceBase>(Mesh->GetAnimInstance());
	if (!AnimInstance) return;

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
	{
		AnimInstance->PlayReleaseAnim();
		break;
	}
	case EGameplayCueEvent::Removed:
	{
		AnimInstance->ReleaseAnimEnd();
		break;
	}
	default:
		break;
	}
}
