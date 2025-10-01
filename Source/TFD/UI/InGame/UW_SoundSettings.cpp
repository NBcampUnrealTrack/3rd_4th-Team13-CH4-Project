#include "TFD/UI/InGame/UW_SoundSettings.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameInstance/TFDGameInstance.h"
#include "Styling/SlateTypes.h"
#include "Components/Border.h"


void UUW_SoundSettings::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->OnValueChanged.AddDynamic(this, &UUW_SoundSettings::OnMasterVolumeChanged);
	}

	if (Slider_BGMVolume)
	{
		Slider_BGMVolume->OnValueChanged.AddDynamic(this, &UUW_SoundSettings::OnBGMVolumeChanged);
	}

	if (Slider_SFXVolume)
	{
		Slider_SFXVolume->OnValueChanged.AddDynamic(this, &UUW_SoundSettings::OnSFXVolumeChanged);
	}

	// 초기값 설정
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
	{
		Slider_MasterVolume->SetValue(GI->MasterVolume);
		UpdateVolumeText(Txt_MasterValue, GI->MasterVolume, TEXT("Master"));
		UpdateGaugeColor(Gauge_Master, GI->MasterVolume); 

		Slider_BGMVolume->SetValue(GI->BGMVolume);
		UpdateVolumeText(Txt_BGMValue, GI->BGMVolume, TEXT("BGM"));
		UpdateGaugeColor(Gauge_BGM, GI->BGMVolume); 

		Slider_SFXVolume->SetValue(GI->SFXVolume);
		UpdateVolumeText(Txt_SFXValue, GI->SFXVolume, TEXT("SFX"));
		UpdateGaugeColor(Gauge_SFX, GI->SFXVolume);
	}

}

// 게이지 색상 갱신 함수
void UUW_SoundSettings::UpdateGaugeColor(UBorder* Gauge, float Value)
{
	if (!Gauge) return;

	float InverseValue = 1.0f - Value;
	FLinearColor GaugeColor = FLinearColor::LerpUsingHSV(FLinearColor::White, FLinearColor::Black, InverseValue);
	Gauge->SetBrushColor(GaugeColor);
}

void UUW_SoundSettings::OnMasterVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
		GI->SetMasterVolume(Value);

	UpdateVolumeText(Txt_MasterValue, Value, TEXT("Master"));
	UpdateGaugeColor(Gauge_Master, Value);
}

void UUW_SoundSettings::OnBGMVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
		GI->SetBGMVolume(Value);

	UpdateVolumeText(Txt_BGMValue, Value, TEXT("BGM"));
	UpdateGaugeColor(Gauge_BGM, Value);
}

void UUW_SoundSettings::OnSFXVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
		GI->SetSFXVolume(Value);

	UpdateVolumeText(Txt_SFXValue, Value, TEXT("SFX"));
	UpdateGaugeColor(Gauge_SFX, Value);
}

void UUW_SoundSettings::UpdateVolumeText(UTextBlock* TextBlock, float Value, const FString& Label)
{
	if (TextBlock)
	{
		int32 Percent = FMath::RoundToInt(Value * 100.0f);
		FString DisplayText = FString::Printf(TEXT("%s: %d%%"), *Label, Percent);
		TextBlock->SetText(FText::FromString(DisplayText));
	}
}




