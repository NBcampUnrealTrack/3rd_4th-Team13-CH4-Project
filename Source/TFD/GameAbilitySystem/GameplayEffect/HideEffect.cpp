// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayEffect/HideEffect.h"

#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"

UHideEffect::UHideEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

    
    InheritableOwnedTagsContainer.Added.AddTag(TAG_Character_State_Hiding);
    InheritableOwnedTagsContainer.ApplyTo(InheritableOwnedTagsContainer.Added);

	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.f));


    FGameplayEffectCue HidingCue(
        TAG_GameplayCue_Hiding, // Cue Tag
        0.f, // MinLevel
        1.f  // MaxLevel
    );

    GameplayCues.Add(HidingCue);

}

