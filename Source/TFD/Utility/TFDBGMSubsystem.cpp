// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Utility/TFDBGMSubsystem.h"

#include "GameInstance/TFDGameInstance.h"


void UTFDBGMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	if (UWorld* World = GetWorld())
	{
		if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(World->GetGameInstance()))
		{
			FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(World);
			OnLevelChanged(CurrentLevel);
		}
	}
}

void UTFDBGMSubsystem::Deinitialize()
{
	StopBGM();
	BGMComponent = nullptr;

	Super::Deinitialize();
}

void UTFDBGMSubsystem::PlayBGM(USoundBase* BGM, float FadeInTime)
{
	if (!BGM)
		return;

	UWorld* World = GetWorld();

	if (!World)
		return;

	// 이전 BGM 정리
	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent->DestroyComponent();
		BGMComponent = nullptr;
	}

	// SpawnSound2D로 안전하게 재생
	BGMComponent = UGameplayStatics::SpawnSound2D(World, BGM, 1.f, 1.f, 0.f, nullptr, true);
	if (BGMComponent)
	{
		BGMComponent->FadeIn(FadeInTime);
	}
}

void UTFDBGMSubsystem::StopBGM()
{
	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent->DestroyComponent();
		BGMComponent = nullptr;
	}
}

void UTFDBGMSubsystem::PlayUISound(USoundBase* Sound)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
		return;

	if (Sound)
	{
		UGameplayStatics::PlaySound2D(World, Sound);
	}
}

void UTFDBGMSubsystem::OnLevelChanged(const FName& NewLevelName)
{
	FString MapName = NewLevelName.ToString();

	if (MapName.StartsWith(TEXT("UEDPIE_")))
	{
		int32 LastUnderscore;
		if (MapName.FindLastChar('_', LastUnderscore))
		{
			MapName = MapName.RightChop(LastUnderscore + 1);
		}
	}

	UWorld* World = GetWorld();
	if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(World->GetGameInstance()))
	{
		const TArray<FLevelBGMData> GI_MapBGMs = GI->GetMapBGMs();
		for (const FLevelBGMData& Data : GI_MapBGMs)
		{
			if (Data.MapName.ToString() == MapName)
			{
				PlayBGM(Data.BGMSound);
				return;
			}
		}
	}

	// 매칭되는 BGM 없으면 정지
	StopBGM();
}
