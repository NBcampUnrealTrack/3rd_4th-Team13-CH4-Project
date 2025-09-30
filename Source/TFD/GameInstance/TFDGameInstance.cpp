// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/TFDGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Utility/TFDBGMSubsystem.h"

void UTFDGameInstance::Init()
{
	Super::Init();

	// 레벨 변경 시 호출될 함수 바인딩
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTFDGameInstance::OnPostLoadMap);

	
}

void UTFDGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UTFDGameInstance::HandleLevelChanged(const FName& LevelName)
{
	if (UTFDBGMSubsystem* BGM = GetSubsystem<UTFDBGMSubsystem>())
	{
		BGM->OnLevelChanged(LevelName);
	}
}

void UTFDGameInstance::PlayUISound(EUISoundType SoundType)
{

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound: World is nullptr"));
		return;
	}

	// UISoundMap에서 Sound 찾기
	USoundBase** FoundSound = UISoundsMap.Find(SoundType);
	if (!FoundSound || !*FoundSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound: Sound not found for %d"), static_cast<int32>(SoundType));
		return;
	}

	USoundBase* SoundToPlay = *FoundSound;

	// SoundClass 볼륨 확인
	float ClassVolume = 1.f;
	if (SoundToPlay->GetSoundClass())
	{
		ClassVolume = SoundToPlay->GetSoundClass()->Properties.Volume;
	}

	// 최종 볼륨 (Subsystem/GameInstance에서 곱하는 경우)
	float VolumeMultiplier = 1.f; // 필요하면 UISFXVolume 곱하기
	float FinalVolume = VolumeMultiplier * ClassVolume;

	UE_LOG(LogTemp, Warning, TEXT("PlayUISound: %s, ClassVolume: %f, FinalVolume: %f"), 
		*SoundToPlay->GetName(), ClassVolume, FinalVolume);

	if (FinalVolume <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound: Volume is 0, sound will not play"));
		return;
	}

	// 실제 재생
	UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay, FinalVolume);
	
	// if (!GetWorld())
	// 	return;
	// USoundBase* TestSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/UI/Sounds/Click_Cue.Click_Cue"));
	// UGameplayStatics::PlaySound2D(GetWorld(), TestSound);
	//
	// if (USoundBase** FoundSound = UISoundsMap.Find(SoundType))
	// {
	// 	if (*FoundSound && GetWorld())
	// 	{
	// 		UGameplayStatics::PlaySound2D(GetWorld(), *FoundSound);
	// 	}
	// }
}


const TMap<EUISoundType, USoundBase*>& UTFDGameInstance::GetUISounds()
{
	return UISoundsMap;
}

const TArray<FLevelBGMData> UTFDGameInstance::GetMapBGMs()
{
	return MapBGMs;
}

void UTFDGameInstance::OnPostLoadMap(UWorld* World)
{
	if (World)
	{
		FName LevelName = World->GetFName();
		HandleLevelChanged(LevelName);
	}
}
