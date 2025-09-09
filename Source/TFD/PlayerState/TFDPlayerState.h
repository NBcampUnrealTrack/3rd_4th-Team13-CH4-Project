// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TFDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	EPlayerRole PlayRole;
};
