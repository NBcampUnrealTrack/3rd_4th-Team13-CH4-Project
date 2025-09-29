// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TFDGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FLevelBGMData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BGMSound;
};

/**
 * 
 */
UCLASS()
class TFD_API UTFDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void HandleLevelChanged(const FName& LevelName);

	const TArray<FLevelBGMData> GetMapBGMs();

	void OnPostLoadMap(UWorld* World);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TArray<FLevelBGMData> MapBGMs;
};
