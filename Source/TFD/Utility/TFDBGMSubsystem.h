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


	/** ============ UI Sound ============ */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayUISound(USoundBase* Sound);


	void OnLevelChanged(const FName& NewLevelName);

private:
	UPROPERTY()
	ATFDBGMManager* BGMManagerActor = nullptr;

	ATFDBGMManager* FindOrSpawnBGMManager();
	void EnsureBGMManager();

	FTimerHandle LevelChangeTimerHandle;
};
