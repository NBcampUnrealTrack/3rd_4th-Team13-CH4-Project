// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TFDGameplayAbility.generated.h"

class UStopMovementEffect;
/**
 * 
 */
UCLASS()
class TFD_API UTFDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTFDGameplayAbility();

protected:	
	UFUNCTION(BlueprintCallable, Category = "Ability|Effects")
	void ApplyStopEffect(AActor* Target, float Duration);

	UPROPERTY(EditDefaultsOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> StopEffectClass;
};
