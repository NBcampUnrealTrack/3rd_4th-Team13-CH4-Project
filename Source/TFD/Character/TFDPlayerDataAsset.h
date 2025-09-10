// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TFDNativeGameplayTags.h"
#include "Abilities/GameplayAbility.h"
#include "InputMappingContext.h"
#include "TFDPlayerDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FInputActionTagMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;
};

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

	// Ability 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input|Job")
	TObjectPtr<UInputMappingContext> JobMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Input|Job")
	TArray<FInputActionTagMapping> Actions;

};
