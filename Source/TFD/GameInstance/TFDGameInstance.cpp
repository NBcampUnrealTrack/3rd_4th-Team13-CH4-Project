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
		return;
	}

	USoundBase** FoundSound = UISoundsMap.Find(SoundType);
	if (!FoundSound || !*FoundSound)
	{
		return;
	}

	// CreateSound2D + bAutoDestroy = 자동 관리
	UAudioComponent* AudioComp = UGameplayStatics::CreateSound2D(
		GetWorld(), 
		*FoundSound, 
		SFXVolume,
		1.0f,  // Pitch
		0.0f,  // StartTime
		nullptr,  // Concurrency
		true,  // PersistAcrossLevelTransition - 레벨넘어가도 유지시키기
		true   // bAutoDestroy - 재생 완료 후 자동 삭제
	);
    
	if (AudioComp)
	{
		AudioComp->Play();
	}
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
