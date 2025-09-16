// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_HitReact.generated.h"

UCLASS()
class TFD_API AGameplayCue_HitReact : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

protected:
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
