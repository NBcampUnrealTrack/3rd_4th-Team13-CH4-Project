#include "TFD/UI/InGame/UW_SoundSettings.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameInstance/TFDGameInstance.h"

void UUW_SoundSettings::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->SetValue(0.5f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Slider_MasterVolume is nullptr!"));
	}

	if (Slider_BGMVolume)
	{
		Slider_BGMVolume->SetValue(0.5f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Slider_BGMVolume is nullptr!"));
	}

	if (Slider_SFXVolume)
	{
		Slider_SFXVolume->SetValue(0.5f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Slider_SFXVolume is nullptr!"));
	}


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
		UpdateVolumeText(Txt_MasterValue, GI->MasterVolume);

		Slider_BGMVolume->SetValue(GI->BGMVolume);
		UpdateVolumeText(Txt_BGMValue, GI->BGMVolume);

		Slider_SFXVolume->SetValue(GI->SFXVolume);
		UpdateVolumeText(Txt_SFXValue, GI->SFXVolume);
	}

}

void UUW_SoundSettings::OnMasterVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
	{
		GI->SetMasterVolume(Value);
	}
	UpdateVolumeText(Txt_MasterValue, Value);
}

void UUW_SoundSettings::OnBGMVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
	{
		GI->SetBGMVolume(Value);
	}
	UpdateVolumeText(Txt_BGMValue, Value);
}

void UUW_SoundSettings::OnSFXVolumeChanged(float Value)
{
	if (UTFDGameInstance* GI = GetWorld()->GetGameInstance<UTFDGameInstance>())
	{
		GI->SetSFXVolume(Value);
	}
	UpdateVolumeText(Txt_SFXValue, Value);
}

void UUW_SoundSettings::UpdateVolumeText(UTextBlock* TextBlock, float Value)
{
	if (TextBlock)
	{
		int32 Percent = FMath::RoundToInt(Value * 100.0f);
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percent)));
	}
}