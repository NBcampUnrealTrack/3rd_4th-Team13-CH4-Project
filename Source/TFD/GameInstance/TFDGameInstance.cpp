// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/TFDGameInstance.h"
#include "Utility/TFDBGMSubsystem.h"

void UTFDGameInstance::Init()
{
	Super::Init();
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

const TArray<FLevelBGMData> UTFDGameInstance::GetMapBGMs()
{
	return MapBGMs;
}
