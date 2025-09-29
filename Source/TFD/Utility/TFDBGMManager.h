// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "TFDBGMManager.generated.h"

UCLASS()
class TFD_API ATFDBGMManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATFDBGMManager();

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* BGM, float FadeInTime = 0.5f);

	// BGM 정지
	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeOutTime = 0.5f);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* BGMComponent;
};
