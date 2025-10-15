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
	void PlayUISound(EUISoundType SoundType);
	
	// BGM 정지
	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeOutTime = 0.5f);

	void OnLevelChanged(const FName& NewLevelName);

	void UpdateVolume();
private:
	UWorld* GetAudioWorld() const;
	
	void EnsureBGMManager();
	
	void EnsureBGMComponent();

private:
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
