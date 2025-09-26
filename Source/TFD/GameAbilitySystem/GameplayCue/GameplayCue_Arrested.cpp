// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCue_Arrested.h"

#include "AnimInstance/TFDAnimInstanceBase.h"
#include "Character/TFDCharacterBase.h"
#include "Character/TFDAICharacter.h"

void AGameplayCue_HitReact::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
                                               const FGameplayCueParameters& Parameters)
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
			EHitDirection Direction = static_cast<EHitDirection>(static_cast<uint8>(Parameters.RawMagnitude));
			AnimInstance->PlayHitAnim(Direction);
			break;
		}
	case EGameplayCueEvent::Removed:
		{
			if (ATFDAICharacter* AIChar = Cast<ATFDAICharacter>(Character))
			{
				// AI면 10초 뒤 AnimInstance::HitAnimEnd + StartMovemnetWalking 실행
				FTimerHandle TimerHandle;
				Character->GetWorldTimerManager().SetTimer(
					TimerHandle,
					FTimerDelegate::CreateLambda([AnimInstance, AIChar]()
						{
							AnimInstance->HitAnimEnd();
						}),
					10.0f,
					false
			);
			}
			else
			{
				// AI가 아니면 즉시 HitAnimEnd 실행
				AnimInstance->HitAnimEnd();
			}
			break;
		}
	default:
		break;
	}
}

