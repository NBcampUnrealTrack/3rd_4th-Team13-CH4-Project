// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UIResourceAsset.h"
#include "Engine/GameInstance.h"
#include "TFDGameInstance.generated.h"


UENUM(BlueprintType)
enum class EUISoundType : uint8
{
	Click_00,
	Click_01,
	Hover,
	OpenMenu,
	CloseMenu,
	Confirm,
	Cancel
};


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


	
	const TMap<EUISoundType, USoundBase*>& GetUISounds();
	const TArray<FLevelBGMData> GetMapBGMs();

	void OnPostLoadMap(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "Audio|Volume")
	void SetMasterVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Audio|Volume")
	void SetBGMVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Audio|Volume")
	void SetSFXVolume(float InVolume);

public:
	UPROPERTY(EditAnywhere)
	TMap<EUISoundType, USoundBase*> UISoundsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TArray<FLevelBGMData> MapBGMs;

	//종합 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Audio|Volume")
	float MasterVolume = 1.f;

	//브금 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Audio|Volume")
	float BGMVolume = 1.f;

	//UI 사운드 & 상호작용,이펙트 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Audio|Volume")
	float SFXVolume = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="UI")
	TSoftObjectPtr<UUIResourceAsset> UIResourceAssetRef; 
	TObjectPtr<UUIResourceAsset> LoadedUIResource = nullptr;

	// Sprite 요청 (비동기 로드 후 콜백 실행)
	void RequestUIIcon(EUIIconType IconType, TFunction<void(TObjectPtr<class UPaperSprite>)> OnLoaded);
};
