// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/PlayingWidget.h"
#include "Components/TextBlock.h"
#include "GameState/TFDGameState.h"
#include "PlayerState/TFDPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "MatchStateWidget.h"
//#include "MatchStateWidget.h"

void UPlayingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] NativeConstruct called."));
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	CachedGameState = PC->GetWorld()->GetGameState<ATFDGameState>();
	if (!CachedGameState) return;

	UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] CachedGameState found: %s"), *CachedGameState->GetName());
	StartGameSec = CachedGameState->GetServerWorldTimeSeconds();
	TotalGameSec = CachedGameState->GetRuleData()->PlayTimeSec;
	
	UpdateRemainingTime();

	GetWorld()->GetTimerManager().SetTimer(
		RemainingTimeHandle,
		this,
		&UPlayingWidget::UpdateRemainingTime,
		0.3f,
		true
	);
}

void UPlayingWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RemainingTimeHandle);
	}
}

void UPlayingWidget::UpdateRemainingTime()
{
	float RemainGameSec = TotalGameSec - (CachedGameState->GetServerWorldTimeSeconds() - StartGameSec);

	int32 RemainMinutes = FMath::FloorToInt(RemainGameSec / 60.f);
	int32 RemainSeconds = FMath::FloorToInt(RemainGameSec) % 60;

	if (!RemainingTimeText) return;

	RemainingTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), RemainMinutes, RemainSeconds)));
}
