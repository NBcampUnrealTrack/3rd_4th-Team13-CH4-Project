// Fill out your copyright notice in the Description page of Project Settings.

#include "TFDAICharacter.h"
#include "TFDNativeGameplayTags.h"
#include "Controller/TFDAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ATFDAICharacter::ATFDAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATFDAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFDAICharacter::StartMovemnetWalking()
{
	if (!HasAuthority()) return;

	if (AbilitySystemComponent)
	{
		TArray<FGameplayAbilitySpec*> Abilities;
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
			FGameplayTagContainer(TAG_Ability_Neutral_RandomMove),
			Abilities
		);

		for (FGameplayAbilitySpec* Spec : Abilities)
		{
			if (Spec)
			{
				AbilitySystemComponent->TryActivateAbility(Spec->Handle);
			}
		}
	}
}

void ATFDAICharacter::StopMovemnetWalking()
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(TAG_Ability_Neutral_RandomMove);

		ASC->CancelAbilities(&CancelTags);
	}
}
