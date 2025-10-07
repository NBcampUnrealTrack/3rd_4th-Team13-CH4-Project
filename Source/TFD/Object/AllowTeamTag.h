// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "AllowTeamTag.generated.h"

//실수로 TFD안붙힘
USTRUCT(BlueprintType)
struct FAllowTeamTag : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
	FGameplayTagContainer		AllowedTeamTag;

	// 스폰할 아이템
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category="Tag")
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category="Tag")
	TArray<TSubclassOf<AActor>> ItemClasses;

	// 스폰할 아이템 주기
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category="Tag")
	float SpawnPeriodSec;

	
	
};