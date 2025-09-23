// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "HideEffect.generated.h"


/**
 * 은신 상태를 부여하는 GameplayEffect
 * - Granted Tags: State.Invisible
 * - Duration: 기본 3초, Ability에서 동적으로 SetDuration 가능
 */
UCLASS()
class TFD_API UHideEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHideEffect();
};
