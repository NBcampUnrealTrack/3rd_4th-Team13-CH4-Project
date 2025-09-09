// Fill out your copyright notice in the Description page of Project Settings.

#include "TFDAICharacter.h"
#include "TFDNativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

ATFDAICharacter::ATFDAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATFDAICharacter::BeginPlay()
{
	Super::BeginPlay();

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
