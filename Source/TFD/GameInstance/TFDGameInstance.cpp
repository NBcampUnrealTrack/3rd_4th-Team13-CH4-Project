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
	if (!GEngine)
	{
		return;
	}

	USoundBase** FoundSound = UISoundsMap.Find(SoundType);
	if (!FoundSound || !*FoundSound)
	{
		return;
	}

	UWorld* CurrentWorld = GEngine->GetWorld();

	// UGameplayStatics::CreateSound2D는 첫 인수로 UWorld*를 받으며,
	// 이 월드 내 임시 액터에 컴포넌트를 붙이게 되어 월드 종속성을 완벽히 제거하긴 어렵습니다.
	// 하지만 GameInstance 레벨에서 TArray로 직접 관리하여 파괴 시점을 제어하는 것이 중요합니다.
	UAudioComponent* AudioComp = UGameplayStatics::CreateSound2D(
		CurrentWorld, // 월드 객체 전달 (nullptr이 아닐 경우)
		*FoundSound,
		SFXVolume,
		1.0f, // Pitch
		0.0f, // StartTime
		nullptr, // Concurrency
		false, // 💡 PersistAcrossLevelTransition: false로 변경해야 합니다. 유지시키지 않습니다.
		false // 💡 bAutoDestroy: false로 변경합니다. 우리가 직접 파괴할 것입니다.
	);

	if (AudioComp)
	{
		// 💡 1. WeakPtr를 사용하여 AudioComp를 안전하게 캡처합니다. (컴포넌트가 파괴된 후에도 접근 시 크래시 방지)
		AudioComp->OnAudioFinished.AddDynamic(this, &UTFDGameInstance::OnUISoundFinished);
       
		ActiveUISounds.Add(AudioComp);
       
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

void UTFDGameInstance::OnUISoundFinished()
{
	for (int32 i = ActiveUISounds.Num() - 1; i >= 0; --i)
	{
		UAudioComponent* Comp = ActiveUISounds[i];
        
		// 널 포인터 체크는 항상 필수
		if (Comp)
		{
			// 💡 IsPlaying()이 false라면, 재생이 끝났다는 의미입니다.
			// (이 콜백이 호출되었다는 것은 어떤 사운드가 끝났다는 것을 보장하지만,
			// 정확히 어떤 컴포넌트인지 알 수 없으므로 모든 컴포넌트를 검사합니다.)
			if (!Comp->IsPlaying())
			{
				// 1. 델리게이트 해제 (중복 호출 방지)
				Comp->OnAudioFinished.RemoveAll(this);
                
				// 2. 배열에서 제거
				ActiveUISounds.RemoveAt(i);
                
				// 3. 메모리에서 파괴 (월드 종속성 문제 해결)
				Comp->DestroyComponent();

				// OnAudioFinished는 사운드 파일 재생이 끝날 때마다 호출될 수 있으므로,
				// 만약 하나의 사운드만 완료된 것이 확실하다면 여기서 break; 할 수 있습니다.
				// 그러나 안전을 위해 모든 요소를 확인하는 것이 좋습니다.
			}
		}
		else
		{
			// 포인터가 이미 유효하지 않다면(파괴되었다면) 배열에서 제거합니다.
			ActiveUISounds.RemoveAt(i);
		}
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
