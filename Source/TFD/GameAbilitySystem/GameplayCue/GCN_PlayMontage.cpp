// Fill out your copyright notice in the Description page of Project Settings.


#include "GCN_PlayMontage.h"

void AGCN_PlayMontage::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);
	UE_LOG(LogTemp, Log, TEXT("Play Montage, EventType : %d"), EventType);
}
