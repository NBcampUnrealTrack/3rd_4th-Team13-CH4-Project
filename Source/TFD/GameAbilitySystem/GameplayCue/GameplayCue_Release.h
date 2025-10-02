// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_Release.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API AGameplayCue_Release : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
