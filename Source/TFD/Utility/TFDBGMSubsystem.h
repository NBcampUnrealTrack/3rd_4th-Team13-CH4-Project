// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	virtual void Deinitialize() override;

	/** ============ BGM ============ */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayBGM(USoundBase* BGM, float FadeInTime = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopBGM();

	/** ============ UI Sound ============ */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayUISound(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void OnLevelChanged(const FName& NewLevelName);
private:
	/** BGM용 오디오 컴포넌트 */
	UPROPERTY()
	UAudioComponent* BGMComponent;


	/** 맵 이름과 연결된 BGM 리스트 (에디터에서 세팅) */
	
};
