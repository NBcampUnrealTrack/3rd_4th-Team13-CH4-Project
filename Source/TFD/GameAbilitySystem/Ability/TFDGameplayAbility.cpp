// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"

#include "GameAbilitySystem/GameplayEffect/StopMovementEffect.h"
#include "AbilitySystemComponent.h"
#include "Character/TFDCharacterBase.h"

#include "UObject/ConstructorHelpers.h"

UTFDGameplayAbility::UTFDGameplayAbility()
{
    static ConstructorHelpers::FClassFinder<UGameplayEffect> EffectBPClass(TEXT("/Game/BluePrint/Ability/GamePlayEffect/GE_StopMovementEffect.GE_StopMovementEffect_C"));
    if (EffectBPClass.Succeeded())
    {
        StopEffectClass = EffectBPClass.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load GE_StopMovementEffect Blueprint!"));
    }
}

void UTFDGameplayAbility::ApplyStopEffect(AActor* Target, float Duration)
{
    if (!Target || !StopEffectClass) return;

    // 대상의 AbilitySystemComponent 가져오기
    ATFDCharacterBase* TargetCH = Cast<ATFDCharacterBase>(Target);

    if (!TargetCH) return;

    UAbilitySystemComponent* ASC = TargetCH->GetAbilitySystemComponent();
    if (!ASC) return;

    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StopEffectClass, 1.0f, Context);
    if (!SpecHandle.IsValid()) return;
    UE_LOG(LogTemp, Warning, TEXT("ApplyStopEffect Excution"));
    SpecHandle.Data->SetDuration(Duration, true);
    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

