// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TFDPlayerDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UTFDPlayerDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// 캐릭터의 초기 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Health;

	// 캐릭터의 최대 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;
    
	// 추가적인 스탯들을 여기에 선언
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Mana;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxMana;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Gold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTag TeamTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer InitialStateTags;

};
