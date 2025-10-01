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

	// EnsureBGMManager();
	EnsureBGMComponent();
	// 첫 맵도 강제 처리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(InitTempHandle, [this]()
		{
			if (UWorld* TimerWorld = GetWorld())
			{
				FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(TimerWorld, true);
				OnLevelChanged(CurrentLevelName);
			}
		}, 0.2f, false);
	}
}

void UTFDBGMSubsystem::PlayBGM(USoundBase* BGM, float FadeInTime)
{
	if (!BGM)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayBGM called with null Sound"));
		return;
	}


	if (!BGMComponent)
	{
		BGMComponent = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass());
		if (BGMComponent)
		{
			BGMComponent->RegisterComponent();
			BGMComponent->bAutoActivate = false;
			BGMComponent->bIsUISound = false;
			BGMComponent->SetVolumeMultiplier(1.0f);
		}
	}

	UWorld* World = GetWorld();
	if (!World || !BGMComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayBGM failed: World or BGMComponent invalid"));
		return;
	}

	if (CurrentPlayingBGM == BGM && BGMComponent->IsPlaying())
	{
		UE_LOG(LogTemp, Log, TEXT("PlayBGM skipped: Same BGM is already playing."));
		return;
	}

	if (BGMComponent->IsPlaying())
	{
		UE_LOG(LogTemp, Log, TEXT("PlayBGM skipped: Same BGM is already playing."));
		BGMComponent->Stop();
	}

	// 현재 재생 중인 BGM 갱신
	CurrentPlayingBGM = BGM;

	// 0.2초 정도 딜레이 후 재생 → 첫 맵 AudioDevice 초기화 문제 방지
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [this, BGM]()
	{
		if (!BGMComponent)
			return;

		BGMComponent->SetSound(BGM);
		BGMComponent->Play();

		UE_LOG(LogTemp, Warning, TEXT("PlayBGM -> Now Playing: %s, IsPlaying=%d"),
		       *BGM->GetName(),
		       BGMComponent->IsPlaying() ? 1 : 0);
	}, 0.2f, false);
}

void UTFDBGMSubsystem::Play_BGM_02(USoundBase* NewBGM, float FadeInTime)
{
	if (!NewBGM)
		return;

	EnsureBGMComponent();

	// 같은 곡이면 그대로 재생 && BGMComponent->IsPlaying()
	if (CurrentPlayingBGM == NewBGM)
	{
		BGMComponent->UnregisterComponent();
		BGMComponent->RegisterComponentWithWorld(GetWorld());
		// BGMComponent->SetSound(CurrentPlayingBGM);
		BGMComponent->Play();
		return;
	}
	//
	// if (CurrentPlayingBGM == NewBGM && BGMComponent && BGMComponent->IsPlaying())
	// 	return;
	// 레벨 전환으로 World가 바뀌었으면 재등록
	if (BGMComponent && GetWorld() && BGMComponent->GetWorld() != GetWorld())
	{
		BGMComponent->UnregisterComponent();
		BGMComponent->RegisterComponentWithWorld(GetWorld());
	}

	// 다른 BGM이면 FadeOut 후 교체
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(0.2f, 0.f);
	}

	CurrentPlayingBGM = NewBGM;
	BGMComponent->SetSound(NewBGM);


	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PlayBGMTimerHandle, [this, FadeInTime]()
		{
			if (BGMComponent && BGMComponent->IsValidLowLevelFast())
			{
				BGMComponent->FadeIn(FadeInTime);
			}
		}, 0.15f, false);
	}
}

void UTFDBGMSubsystem::PlayUISound(EUISoundType SoundType)
{
	if (!GetWorld()) // Subsystem은 WorldContext가 없을 수도 있음 → 방어 코드
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound failed: No World found."));
		return;
	}
	UTFDGameInstance* TFDGI = Cast<UTFDGameInstance>(GetGameInstance());
	

	const TMap<EUISoundType, USoundBase*>& UISoundsMap = TFDGI->GetUISounds();
	if (USoundBase* const* FoundSound = UISoundsMap.Find(SoundType))
	{
		if (*FoundSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), *FoundSound);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayUISound: SoundType %d is mapped to null SoundBase."), (int32)SoundType);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound: SoundType %d not found in UISoundsMap."), (int32)SoundType);
	}
}

void UTFDBGMSubsystem::StopBGM(float FadeOutTime)
{
	if (BGMComponent)
	{
		BGMComponent->FadeOut(FadeOutTime, 0.f);
		CurrentPlayingBGM = nullptr;
	}
}


void UTFDBGMSubsystem::OnLevelChanged(const FName& NewLevelName)
{
	// EnsureBGMManager();
	EnsureBGMComponent();

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
				if (Data.MapName.ToString() == MapName)
				{
					// 새 레벨에 맞는 BGM 실행
					// PlayBGM(Data.BGMSound);

					Play_BGM_02(Data.BGMSound);

					return;
				}
			}
		}

		// 해당 레벨에 매칭되는 BGM이 없으면 정지
		StopBGM();
	}, 0.2f, false); // 0.2초 딜레이
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
}

void UTFDBGMSubsystem::EnsureBGMComponent()
{
	if (!BGMComponent)
	{
		BGMComponent = NewObject<UAudioComponent>(GetGameInstance());
		BGMComponent->bAutoActivate = false;
		BGMComponent->bAutoDestroy = false;
		BGMComponent->bAllowSpatialization = false;

		if (UWorld* World = GetWorld())
		{
			BGMComponent->RegisterComponentWithWorld(World);
		}
	}
}
