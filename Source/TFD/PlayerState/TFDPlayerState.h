// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "GameData/EGameEnums.h"
#include "TFDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	// DataAsset 설정 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag TeamTag;

public:
	void SetTemaTag(FGameplayTag Tag);
	FGameplayTag GetTemaTag() const;
};
