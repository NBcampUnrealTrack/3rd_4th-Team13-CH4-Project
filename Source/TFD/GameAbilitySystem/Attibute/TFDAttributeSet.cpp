// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Attibute/TFDAttributeSet.h"


#include "Character/TFDCharacter.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


UTFDAttributeSet::UTFDAttributeSet()
{
	// 기본값 설정
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitMana(50.0f);
	InitMaxMana(50.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	InitSpeed(300.0f);
	InitGold(100.0f);
}

void UTFDAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTFDAttributeSet, SkillStock, COND_None, REPNOTIFY_Always);
}

void UTFDAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, Health, OldHealth);
}

void UTFDAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, MaxHealth, OldMaxHealth);
}

void UTFDAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, Mana, OldMana);
}

void UTFDAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, MaxMana, OldMaxMana);
}

void UTFDAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, Stamina, OldStamina);
}

void UTFDAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, MaxStamina, OldMaxStamina);
}

void UTFDAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, Speed, OldSpeed);
}

void UTFDAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, Gold, OldGold);
}

void UTFDAttributeSet::OnRep_SkillStock(const FGameplayAttributeData& OlSkillStock)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTFDAttributeSet, SkillStock, OlSkillStock);
}

void UTFDAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetSpeedAttribute())
    {
        float OldSpeed = GetSpeed();

        // 절대 최대값 제한
        if (NewValue > MaxAllowedSpeed)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[UTFDAttributeSet][PreAttributeChange] Speed clamped: %.2f -> %.2f (Max: %.2f)"),
                NewValue, MaxAllowedSpeed, MaxAllowedSpeed);
            NewValue = MaxAllowedSpeed;
        }

        // 최소값 제한
        if (NewValue < 0.f)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[UTFDAttributeSet][PreAttributeChange] Negative speed prevented: %.2f -> 0"),
                NewValue);
            NewValue = 0.f;
        }

        // 서버에서 급격한 변화 감지
        if (GetOwningActor() && GetOwningActor()->HasAuthority())
        {
            float SpeedChange = FMath::Abs(NewValue - OldSpeed);

            if (SpeedChange > MaxSpeedChangePerFrame)
            {
                UE_LOG(LogTemp, Error,
                    TEXT("[UTFDAttributeSet][PreAttributeChange][AntiCheat] Suspicious speed change! Actor: %s, Old: %.2f, Attempted: %.2f, Change: %.2f (Max: %.2f)"),
                    *GetOwningActor()->GetName(), OldSpeed, NewValue, SpeedChange, MaxSpeedChangePerFrame);

                // 변경 거부
                NewValue = OldSpeed;

                // TODO: 의심 활동 기록 시스템 연동
                // - 누적 카운트
                // - 일정 횟수 이상 시 플레이어 킥
            }
        }
    }
}

void UTFDAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Speed 검증은 PreAttributeChange에서 이미 처리되므로 여기서는 추가 처리 없음
	// 향후 다른 속성에 대한 후처리가 필요하면 여기에 추가
}