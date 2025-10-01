#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "UW_SoundSettings.generated.h" 

class UBorder;
class USlider;
class UTextBlock;

UCLASS()
class TFD_API UUW_SoundSettings : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 슬라이더 바인딩
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_MasterVolume;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_BGMVolume;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_SFXVolume;

	// 텍스트 바인딩
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_MasterValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_BGMValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SFXValue;

	// 게이지 바인딩
	UPROPERTY(meta = (BindWidget))
	UBorder* Gauge_Master;

	UPROPERTY(meta = (BindWidget))
	UBorder* Gauge_BGM;

	UPROPERTY(meta = (BindWidget))
	UBorder* Gauge_SFX;


	// 슬라이더 이벤트
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	// 텍스트 갱신 함수
	void UpdateVolumeText(UTextBlock* TextBlock, float Value, const FString& Label);

	// 게이지 색상 갱신 함수
	void UpdateGaugeColor(UBorder* Gauge, float Value);

};