// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_Demerit.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API AGameplayCue_Demerit : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
