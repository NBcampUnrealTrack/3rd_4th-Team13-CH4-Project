// Fill out your copyright notice in the Description page of Project Settings.

#include "Utility/TFDBGMSubsystem.h"

#include "EngineUtils.h"
#include "Sound/SoundBase.h"
#include "TFDBGMManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

#include "GameInstance/TFDGameInstance.h"


void UTFDBGMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EnsureBGMManager();

	// 첫 맵도 강제 처리
	if (UWorld* World = GetWorld())
	{
		FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(World, true);
		OnLevelChanged(CurrentLevelName);
	}
}


void UTFDBGMSubsystem::OnLevelChanged(const FName& NewLevelName)
{
	EnsureBGMManager();

	UWorld* World = GetWorld();

	if (!World)
		return;

	World->GetTimerManager().SetTimer(LevelChangeTimerHandle, [this, NewLevelName]()
	{
		FString MapName = NewLevelName.ToString();
		if (MapName.StartsWith(TEXT("UEDPIE_")))
		{
			int32 LastUnderscore;
			if (MapName.FindLastChar('_', LastUnderscore))
				MapName = MapName.RightChop(LastUnderscore + 1);
		}

		if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(GetGameInstance()))
		{
			const TArray<FLevelBGMData>& GI_MapBGMs = GI->GetMapBGMs();
			for (const FLevelBGMData& Data : GI_MapBGMs)
			{
				if (Data.MapName.ToString() == MapName && BGMManagerActor)
				{
					BGMManagerActor->PlayBGM(Data.BGMSound);
					return;
				}
			}
		}


		if (BGMManagerActor)
			BGMManagerActor->StopBGM();
	}, 0.2f, false); // 0.2초 딜레이
}

ATFDBGMManager* UTFDBGMSubsystem::FindOrSpawnBGMManager()
{
	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	// 월드에서 기존 BGMManager 찾기
	for (TActorIterator<ATFDBGMManager> It(World); It; ++It)
	{
		return *It;
	}

	// 없으면 Spawn
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return World->SpawnActor<ATFDBGMManager>(
		BGMManagerActor ? BGMManagerActor->GetClass() : ATFDBGMManager::StaticClass(),
		Params);
}

void UTFDBGMSubsystem::EnsureBGMManager()
{
	if (BGMManagerActor && BGMManagerActor->IsValidLowLevel())
		return; // 이미 존재하면 그대로 사용

	UWorld* World = GetWorld();
	if (!World)
		return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	BGMManagerActor = World->SpawnActor<ATFDBGMManager>(
		ATFDBGMManager::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	UE_LOG(LogTemp, Warning, TEXT("BGMManagerActor spawned"));

	// UWorld* World = GetWorld();
	// if (!World)
	// 	return;
	//
	// // 기존 액터가 다른 월드에 속해있거나 nullptr이면 새로 스폰
	// if (!BGMManagerActor || BGMManagerActor->GetWorld() != World)
	// {
	// 	if (BGMManagerActor)
	// 	{
	// 		BGMManagerActor->Destroy();
	// 		BGMManagerActor = nullptr;
	// 	}
	//
	// 	FActorSpawnParameters Params;
	// 	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// 	BGMManagerActor = World->SpawnActor<ATFDBGMManager>(
	// 		ATFDBGMManager::StaticClass(),
	// 		FVector::ZeroVector,
	// 		FRotator::ZeroRotator,
	// 		Params
	// 	);
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("BGMManagerActor spawned for new level"));
	// }
}
