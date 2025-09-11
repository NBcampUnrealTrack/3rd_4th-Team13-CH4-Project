// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TFDGameMode.h"


#include "AIController.h"
#include "EngineUtils.h"
#include "TFDNativeGameplayTags.h"
#include "Character/TFDAICharacter.h"
#include "Character/TFDCharacter.h"
#include "Controller/TFDPlayerController.h"
#include "GameState/TFDGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "Object/TFDSpawnVolume.h"




void ATFDGameMode::BeginPlay()
{
	Super::BeginPlay();
	bUseSeamlessTravel = true;

	InitializeSpawnVolumes();
}

AActor* ATFDGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 스폰 볼륨이 초기화되지 않았다면 초기화
	if (SpawnVolumes.Num() == 0)
	{
		InitializeSpawnVolumes();
		UE_LOG(LogTemp, Display, TEXT("CPS_1"));
	}
	

	ATFDPlayerState* PS = Player ? Player->GetPlayerState<ATFDPlayerState>() : nullptr;
	if (!PS)
		return Super::ChoosePlayerStart_Implementation(Player);

	FGameplayTag PlayerTag = PS->GetTemaTag();
	
	// 랜덤 위치를 얻어서 임시 액터 생성A
	FVector RandomLocation = GetRandomPointInSpawnAreaTag(PlayerTag);
	if (RandomLocation == FVector::ZeroVector)
		return nullptr;
	
	// 2. 엔진이 스폰 위치를 인식할 수 있도록 임시 APlayerStart 액터를 생성합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	APlayerStart* PlayerStartActor = GetWorld()->SpawnActor<APlayerStart>(RandomLocation, FRotator::ZeroRotator, SpawnParams);
	
	// 3. 찾은 위치가 유효한지 확인 후 반환합니다.
	if (PlayerStartActor)
	{
		// 로그로 확인
		UE_LOG(LogTemp, Display, TEXT("CPS: Player will spawn at a new location: %s"), *RandomLocation.ToString());
	      
		return PlayerStartActor;
	}
    
	// 유효한 위치를 찾지 못했다면 부모 클래스의 기본 함수를 호출
	
	return Super::ChoosePlayerStart_Implementation(Player);
	

}

void ATFDGameMode::SpawnAI()
{
	FVector SpawnLoc = GetRandomPointInSpawnArea();
	FRotator SpawnRot = FRotator::ZeroRotator;

	FGameplayTag AITag =TAG_Team_Neutral;
	for (int32 i = 0; i < NumberOfAI; ++i)
	{
		SpawnLoc = GetRandomPointInSpawnAreaTag(AITag);
		UE_LOG(LogTemp, Display, TEXT("Spawning Player X:%.f,Y:%.f,Z:%.f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
		GetWorld()->SpawnActor<ATFDAICharacter>(AICharacter, SpawnLoc, SpawnRot);
	}
}


FVector ATFDGameMode::GetRandomPointInSpawnArea()
{
	FVector RandomPoint = FVector::ZeroVector;
	if (SpawnVolumes.Num() == 0)
		return RandomPoint;

	if (SpawnVolumes.Num() >= 1)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnVolumes.Num() - 1);
		RandomPoint = SpawnVolumes[RandomIndex]->GetRandomPointInVolume();
	}

	return RandomPoint;
}

FVector ATFDGameMode::GetRandomPointInSpawnAreaTag(FGameplayTag InTag)
{
	FVector RandomPoint = FVector::ZeroVector;
	if (SpawnVolumes.Num() == 0)
		return RandomPoint;

	if (SpawnVolumes.Num() >= 1)
	{
		ATFDSpawnVolume* SpawnVolume = GetRandomSpawnVolumeTag(InTag);
		if (SpawnVolume)
			RandomPoint = SpawnVolume->GetRandomPointInVolume();
	}

	return RandomPoint;
}

FVector ATFDGameMode::GetRandomPointInSpawnAreaAI()
{
	FVector RandomPoint = FVector::ZeroVector;
	if (SpawnVolumes.Num() == 0)
		return RandomPoint;

	if (SpawnVolumes.Num() > 1)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnVolumes.Num() - 1);
		RandomPoint = SpawnVolumes[RandomIndex]->GetRandomPointInVolume();
	}

	return RandomPoint;
}

ATFDSpawnVolume* ATFDGameMode::GetRandomSpawnVolume()
{
	if (SpawnVolumes.Num() == 0)
		return nullptr;

	ATFDSpawnVolume* pSpawnVolume = SpawnVolumes[FMath::RandRange(0, SpawnVolumes.Num() - 1)];

	return pSpawnVolume;
}

ATFDSpawnVolume* ATFDGameMode::GetRandomSpawnVolumeTag(FGameplayTag InTag)
{
	if (SpawnVolumes.Num() == 0)
		return nullptr;

	int32 ValidCount = 0;
	for (ATFDSpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (SpawnVolume && SpawnVolume->CheckTeamTag(InTag))
		{
			ValidCount++;
		}
	}

	if (ValidCount == 0)
		return nullptr;

	// 랜덤 인덱스를 생성 (0부터 ValidCount-1까지)
	int32 RandomIndex = FMath::RandRange(0, ValidCount - 1);

	// 유효한 것들 중에서 RandomIndex번째 것을 찾는다
	int32 CurrentValidIndex = 0;
	for (ATFDSpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (SpawnVolume && SpawnVolume->CheckTeamTag(InTag))
		{
			if (CurrentValidIndex == RandomIndex)
			{
				return SpawnVolume;
			}
			CurrentValidIndex++;
		}
	}

	return nullptr;
}


void ATFDGameMode::InitializeSpawnVolumes()
{
	if (SpawnVolumes.Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("IS: SpawnVolumes already "));
		return; // 이미 초기화되서 종료
	}

	// ATFDSpawnVolume 클래스만 직접 찾기
	for (TActorIterator<ATFDSpawnVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ATFDSpawnVolume* SpawnVolume = *ActorItr;
		if (IsValid(SpawnVolume))
		{
			SpawnVolumes.Add(SpawnVolume);
			UE_LOG(LogTemp, Display, TEXT("Found spawn volume: %s"), *SpawnVolume->GetName());
		}
	}
}

void ATFDGameMode::MovePlayerToRandomSpawnPoint(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
		return;

	FVector SpawnLocation = GetRandomPointInSpawnArea();
	UE_LOG(LogTemp, Display, TEXT("random location, X: %.1f,Y:%.1f"), SpawnLocation.X, SpawnLocation.Y);

	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		PlayerPawn->SetActorLocation(SpawnLocation);
		UE_LOG(LogTemp, Display, TEXT("Player moved to random location, X: %.1f,Y:%.1f"), SpawnLocation.X,
		       SpawnLocation.Y);
	}
}


void ATFDGameMode::OnCatchThief(APawn* APawn)
{
}

void ATFDGameMode::OnHandleGameEnd(EGameCompleteType CompleteType)
{
}

void ATFDGameMode::GamePause(bool bIsPaused)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}
	
	// bIsPaused 에 따라서 플레이어, AI 활성 비활성화 하고 있습니다.
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(It->Get()))
		{
			PC->SetMovemnetWalking(!bIsPaused);
		}
	}
	
	// 모든 AI
	for (TActorIterator<AAIController> It(World); It; ++It)
	{
		AAIController* AI = *It;
		if (ATFDAICharacter* WorldAICharacter = Cast<ATFDAICharacter> (AI->GetCharacter()))
		{
			if (!bIsPaused)
			{
				WorldAICharacter->StartMovemnetWalking(); //움직임 활성화
			}
			else
			{
				WorldAICharacter->StopMovemnetWalking(); //움직임 비활성화
			}
			
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Game Start! All players movement enabled."));
}

void ATFDGameMode::PlayerIsReady(AController* PlayerController)
{
	// 모든 플레이어가 준비되었는지 확인하는 등의 로직
	UE_LOG(LogTemp, Warning, TEXT("Player is ready: %s"), *PlayerController->GetName());

	ATFDGameState* TFDGameState = GetGameState<ATFDGameState>();
	ATFDPlayerState* TFDPlayerState = PlayerController->GetPlayerState<ATFDPlayerState>();

	if(TFDGameState && TFDPlayerState)
	{
		TFDGameState->MarkPlayerReady(TFDPlayerState);

		// 모든 플레이어가 접속했는지 확인 (GameState의 PlayerArray 사용)
		if(TFDGameState->GetReadyPlayerCount() >= TFDGameState->PlayerArray.Num() && TFDGameState->PlayerArray.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("All players are ready. Starting the game."));
			GamePause(false);
		}
	}
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
	// AI스폰
	
	// 팀 비율 결정
	
	// 결정된 수에 따라 플레이어 character 스폰
	// PlayerController 배열 가져오기
	
}

