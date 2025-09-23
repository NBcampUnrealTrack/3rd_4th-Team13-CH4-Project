// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_Hiding.generated.h"

/**
 * 은신 Cue
 * - GE의 "State.Hiding" 태그가 붙으면 실행
 * - Mesh 숨기기, Collision 비활성화
 */
UCLASS()
class TFD_API AGameplayCue_Hiding : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
