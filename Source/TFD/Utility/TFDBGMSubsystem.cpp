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
	if (!World || !World->IsGameWorld())
		return;
	
	if (BGMComponent)
	{
		if (BGMComponent->IsPlaying())
		{
			BGMComponent->Stop();
		}
		
		if (BGMComponent->IsRegistered())
		{
			BGMComponent->UnregisterComponent();
		}

		if (BGMComponent->IsValidLowLevel())
		{
			BGMComponent->DestroyComponent();
		}
		BGMComponent = nullptr;
	}

	// 새로 생성
	BGMComponent = NewObject<UAudioComponent>(World);
	if (!BGMComponent)
		return;

	BGMComponent->bAutoActivate = false;
	BGMComponent->bAutoDestroy = false;
	BGMComponent->bAllowSpatialization = false;

	if (CurrentPlayingBGM == BGM)
	{
		return;
	}

	CurrentPlayingBGM = BGM;
	BGMComponent->SetSound(BGM);
	if (!BGMComponent->IsRegistered())
	{
		BGMComponent->RegisterComponentWithWorld(World);
	}

	if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(GetGameInstance()))
	{
		float FinalVolume = GI->MasterVolume * GI->BGMVolume;
		BGMComponent->SetVolumeMultiplier(FinalVolume);
	}

	BGMComponent->Play();
}

void UTFDBGMSubsystem::PlayUISound(EUISoundType SoundType)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayUISound failed: No World found."));
		return;
	}
	UTFDGameInstance* TFDGI = Cast<UTFDGameInstance>(GetGameInstance());

	if (!TFDGI)
	{
		return;
	}

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
	if (!BGMComponent)
		return;
	
	if (BGMComponent->IsPlaying())
	{
		if (FadeOutTime > 0.f)
		{
			BGMComponent->FadeOut(FadeOutTime, 0.f);
		}
		else
		{
			BGMComponent->Stop();
		}
	}
	
	if (BGMComponent->IsRegistered())
	{
		BGMComponent->UnregisterComponent();
	}
	if (BGMComponent->IsValidLowLevel())
	{
		BGMComponent->DestroyComponent();
	}
	BGMComponent = nullptr;
	CurrentPlayingBGM = nullptr;
}


void UTFDBGMSubsystem::OnLevelChanged(const FName& NewLevelName)
{
	UWorld* World = GetWorld();

	if (!World)
		return;

	StopBGM();

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
		UWorld* GIWorld = GI->GetWorld();


		if (GIWorld && GIWorld->IsGameWorld())
		{
			return GIWorld;
		}

	
		if (const APlayerController* PC = GI->GetFirstLocalPlayerController())
		{
			if (UWorld* PCWorld = PC->GetWorld())
			{
				return PCWorld;
			}
		}
	}

	// 최후의 보루: 전역 GWorld
	return GEngine ? GEngine->GetWorldContexts()[0].World() : nullptr;
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