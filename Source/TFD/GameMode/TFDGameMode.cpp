// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TFDGameMode.h"

void ATFDGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 레벨 로드 완료 대기
}

void ATFDGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// 남은 시간 업데이트 처리
	// 시간이 종료될 시 게임 종료 로직
}

void ATFDGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	// PlayerController 배열 가져오기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// 원하는 Pawn 스폰
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			// APawn* NewPawn = GetWorld()->
			// 	SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

			// Possess
			// if (NewPawn)
			// {
			// 	PC->Possess(NewPawn);
			// }
		}
	}
}
