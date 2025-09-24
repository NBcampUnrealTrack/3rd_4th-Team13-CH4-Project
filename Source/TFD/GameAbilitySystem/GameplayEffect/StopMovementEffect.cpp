// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayEffect/StopMovementEffect.h"

#include "TFDNativeGameplayTags.h"

#include "GameplayTagContainer.h"


UStopMovementEffect::UStopMovementEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.f));
}

