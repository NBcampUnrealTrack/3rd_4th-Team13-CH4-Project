// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFDCharacterBase.h"
#include "TFDAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDAICharacter : public ATFDCharacterBase
{
	GENERATED_BODY()
public:
	ATFDAICharacter();

protected:
	virtual void BeginPlay() override;
};
