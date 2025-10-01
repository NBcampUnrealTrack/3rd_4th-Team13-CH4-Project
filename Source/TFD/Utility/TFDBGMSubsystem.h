// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFDBGMManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "TFDBGMSubsystem.generated.h"



/**
 * 
 */
UCLASS()
class TFD_API UTFDBGMSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* BGM, float FadeInTime = 0.5f);

	UFUNCTION(BlueprintCallable)
	void Play_BGM_02(USoundBase* NewBGM, float FadeInTime = 0.5f);

	
	// BGM 정지
	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeOutTime = 0.5f);

	void OnLevelChanged(const FName& NewLevelName);

private:
	void EnsureBGMManager();
	
	void EnsureBGMComponent();

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* BGMComponent;
	
	UPROPERTY()
	ATFDBGMManager* BGMManagerActor = nullptr;
	
	UPROPERTY()
	USoundBase* CurrentPlayingBGM;
	
	FTimerHandle LevelChangeTimerHandle;
	FTimerHandle InitTempHandle;
	FTimerHandle PlayBGMTimerHandle;
	bool bFirstMap = true;
};
