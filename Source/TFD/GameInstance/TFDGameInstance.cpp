// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/TFDGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Utility/TFDBGMSubsystem.h"

void UTFDGameInstance::Init()
{
	Super::Init();

	// 레벨 변경 시 호출될 함수 바인딩
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTFDGameInstance::OnPostLoadMap);
}

void UTFDGameInstance::OnStart()
{
	Super::OnStart();
	// 레벨 변경 시 호출될 함수 바인딩
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTFDGameInstance::OnPostLoadMap);

}

void UTFDGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UTFDGameInstance::HandleLevelChanged(const FName& LevelName)
{
	if (!IsValid(this))
	{
		return;
	}

	if (GetWorld() == nullptr)
	{
		return;
	}
	
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
		1.0f, // Pitch
		0.0f, // StartTime
		nullptr, // Concurrency
		true, // PersistAcrossLevelTransition - 레벨넘어가도 유지시키기
		true // bAutoDestroy - 재생 완료 후 자동 삭제
	);

	if (AudioComp)
	{
		AudioComp->Play();
	}
}

void UTFDGameInstance::SetMasterVolume(float InVolume)
{
	MasterVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);
}

void UTFDGameInstance::SetBGMVolume(float InVolume)
{
	BGMVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);
}

void UTFDGameInstance::SetSFXVolume(float InVolume)
{
	SFXVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);
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
	if (!IsValid(this))
	{
		return;
	}

	if (!World)
	{
		return;
	}

	// PIE나 에디터 전용 월드 같은 경우 필터링
	if (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::Inactive)
	{
		return;
	}

	if (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::Inactive)
	{
		return;
	}

	

	FName LevelName = World->GetFName();
	if (LevelName.IsNone())
	{
		return;
	}
	
	HandleLevelChanged(LevelName);
	// if (World)
	// {
	// 	FName LevelName = World->GetFName();
	// 	HandleLevelChanged(LevelName);
	// }
}
