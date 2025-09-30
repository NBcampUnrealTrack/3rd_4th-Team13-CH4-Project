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

