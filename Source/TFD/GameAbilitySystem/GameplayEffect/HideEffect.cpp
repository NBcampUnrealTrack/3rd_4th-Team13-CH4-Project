// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayEffect/HideEffect.h"

#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UHideEffect::UHideEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	// 컴포넌트 생성 및 등록
	UTargetTagsGameplayEffectComponent* TargetTags =
		CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTags"));
	GEComponents.Add(TargetTags); // 컴포넌트를 GE에 등록

	// 태그 설정 후 적용
	FInheritedTagContainer TagChanges = TargetTags->GetConfiguredTargetTagChanges();
	TagChanges.Added.AddTag(TAG_Character_State_Hiding);
	TargetTags->SetAndApplyTargetTagChanges(TagChanges);
    
    //InheritableOwnedTagsContainer.Added.AddTag(TAG_Character_State_Hiding);
    // InheritableOwnedTagsContainer.ApplyTo(InheritableOwnedTagsContainer.Added);

	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.f));


    FGameplayEffectCue HidingCue(
        TAG_GameplayCue_Hiding, // Cue Tag
        0.f, // MinLevel
        1.f  // MaxLevel
    );

    GameplayCues.Add(HidingCue);

}

