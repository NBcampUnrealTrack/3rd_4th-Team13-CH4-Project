// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InGameUtility.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UInGameUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Game|Roles")
	static int GetPoliceRoleCount(int32 NumPlayers);
};
