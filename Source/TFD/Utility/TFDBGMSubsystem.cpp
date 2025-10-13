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
	//EnsureBGMComponent();
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
		return;

	UWorld* World = GetAudioWorld();
	if (!World)
		return; // 월드가 없으면 안전하게 리턴

	// 이전 BGMComponent가 존재하면, 월드가 바뀌었을 수 있으므로 새로 등록
	if (!BGMComponent || !BGMComponent->IsValidLowLevelFast() || BGMComponent->GetWorld() != World)
	{
		if (BGMComponent)
		{
			BGMComponent->Stop();
			BGMComponent->DestroyComponent();
		}

		BGMComponent = NewObject<UAudioComponent>(GetGameInstance());
		BGMComponent->bAutoDestroy = false;
		BGMComponent->bAllowSpatialization = false;
		BGMComponent->RegisterComponentWithWorld(World);
	}

	// 이미 같은 곡이 재생 중이면 아무것도 안함
	if (CurrentPlayingBGM == BGM && BGMComponent->IsPlaying())
		return;

	CurrentPlayingBGM = BGM;
	BGMComponent->SetSound(BGM);

	if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(GetGameInstance()))
	{
		float FinalVolume = GI->MasterVolume * GI->BGMVolume;
		BGMComponent->SetVolumeMultiplier(FinalVolume);
	}
	BGMComponent->Play();
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
			float FinalVolume = TFDGI->MasterVolume * TFDGI->SFXVolume;
			UGameplayStatics::PlaySound2D(GetWorld(), *FoundSound, FinalVolume);
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
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->Stop();
		CurrentPlayingBGM = nullptr;
	}
}


void UTFDBGMSubsystem::OnLevelChanged(const FName& NewLevelName)
{
	UWorld* World = GetWorld();

	if (!World)
		return;

	// 기존 타이머 제거 후 새로 설정
	World->GetTimerManager().ClearTimer(LevelChangeTimerHandle);

	FTimerDelegate TimerDel;
	TWeakObjectPtr<UTFDBGMSubsystem> WeakThis(this);

	TimerDel.BindLambda([WeakThis, NewLevelName]()
	{
		if (!WeakThis.IsValid())
			return;

		UWorld* InnerWorld = WeakThis->GetWorld();
		if (!InnerWorld)
			return;

		FString MapName = NewLevelName.ToString();
		if (MapName.StartsWith(TEXT("UEDPIE_")))
		{
			int32 LastUnderscore;
			if (MapName.FindLastChar('_', LastUnderscore))
				MapName = MapName.RightChop(LastUnderscore + 1);
		}

		if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(WeakThis->GetGameInstance()))
		{
			const TArray<FLevelBGMData>& GI_MapBGMs = GI->GetMapBGMs();
			for (const FLevelBGMData& Data : GI_MapBGMs)
			{
				if (Data.MapName.ToString() == MapName)
				{
					WeakThis->PlayBGM(Data.BGMSound);
					return;
				}
			}
		}

		// 해당 레벨에 매칭되는 BGM이 없으면 정지
		WeakThis->StopBGM();
	});

	World->GetTimerManager().SetTimer(LevelChangeTimerHandle, TimerDel, 0.2f, false);
}

void UTFDBGMSubsystem::UpdateVolume()
{
	if (BGMComponent && CurrentPlayingBGM)
	{
		if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(GetGameInstance()))
		{
			BGMComponent->SetVolumeMultiplier(GI->MasterVolume * GI->BGMVolume);
		}
	}
}

UWorld* UTFDBGMSubsystem::GetAudioWorld() const
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		// GameInstance는 여러 World를 가질 수 있음
		// GetWorldContexts를 통해 현재 활성 월드를 가져오는 것이 가장 안전함
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.World() && Context.WorldType == EWorldType::Game)
			{
				return Context.World();
			}
		}
	}
	// if (const UGameInstance* GI = GetGameInstance())
	// {
	// 	// 로컬 플레이어가 존재하면, 그 월드를 우선 사용
	// 	if (GI->GetFirstLocalPlayerController())
	// 	{
	// 		return GI->GetFirstLocalPlayerController()->GetWorld();
	// 	}
	// 	// 아니면 기본 World 반환
	// 	return GI->GetWorld();
	// }
	return nullptr;
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
