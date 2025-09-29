// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/TFDBGMManager.h"

// Sets default values
ATFDBGMManager::ATFDBGMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	BGMComponent->bAutoActivate = false;
	BGMComponent->bIsUISound = false;
	BGMComponent->SetupAttachment(RootComponent);
	// RootComponent = BGMComponent;
}

void ATFDBGMManager::PlayBGM(USoundBase* BGM, float FadeInTime)
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

	// 0.2초 정도 딜레이 후 재생 → 첫 맵 AudioDevice 초기화 문제 방지
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [this, BGM]()
	{
		if (!BGMComponent) return;

		BGMComponent->SetSound(BGM);
		BGMComponent->Play();

		UE_LOG(LogTemp, Warning, TEXT("PlayBGM -> Now Playing: %s, IsPlaying=%d"),
			*BGM->GetName(),
			BGMComponent->IsPlaying() ? 1 : 0);

	}, 0.2f, false);
	

}

void ATFDBGMManager::StopBGM(float FadeOutTime)
{
	if (BGMComponent)
	{
		BGMComponent->FadeOut(FadeOutTime, 0.f);
	}
}

// Called when the game starts or when spawned
void ATFDBGMManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATFDBGMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

