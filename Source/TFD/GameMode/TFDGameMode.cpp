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
#include "Utility/InGameUtility.h"


ATFDGameMode::ATFDGameMode()
{
	DefaultPawnClass = nullptr;
	bUseSeamlessTravel = true;
}

void ATFDGameMode::BeginPlay()
{
	Super::BeginPlay();
}

APawn* ATFDGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	if (!NewPlayer || !StartSpot) return nullptr;

	ATFDPlayerState* PS = NewPlayer->GetPlayerState<ATFDPlayerState>();

	if (!PS || PS->GetTeamTag() == FGameplayTag::EmptyTag)
		return nullptr;
	
	// Pawn 클래스 가져오기
	TSubclassOf<APawn> PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass) return nullptr;

	// SpawnTransform 설정
	FTransform SpawnTransform = StartSpot->GetActorTransform();
	FVector SpawnLocation = GetRandomPointInSpawnAreaTag(PS->GetTeamTag());
	SpawnTransform.SetLocation( SpawnLocation);

	// SpawnActorDeferred로 Pawn 생성
	ATFDCharacterBase* Pawn = GetWorld()->SpawnActorDeferred<ATFDCharacterBase>(PawnClass, SpawnTransform, NewPlayer,
		nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!Pawn) return nullptr;
	
	const FGameRuleData& RuleData = GetGameState()->GetRuleData();
	ATFDPlayerState* PState = Cast<ATFDPlayerState>(NewPlayer->PlayerState);
	ATFDCharacterBase*  TFDPawn = Cast<ATFDCharacterBase>(Pawn);
	if (PState && TFDPawn)
	{
		FGameplayTag Tag = PState->GetTeamTag();
		if (Tag == TAG_Team_Thief)
		{
			TFDPawn->CharacterData = RuleData.ThiefDataAsset;
		}
		else if (Tag == TAG_Team_Cop)
		{
			TFDPawn->CharacterData = RuleData.PoliceDataAsset;
		}
	}
	// FinishSpawningActor 호출 (BeginPlay 직전)
	UGameplayStatics::FinishSpawningActor(TFDPawn, SpawnTransform);
	return Pawn;
}


bool ATFDGameMode::ReadyToStartMatch_Implementation()
{
	return GetGameState()->GetCurrentGameState() == EGameState::Playing;
}

void ATFDGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	InitializeSpawnVolumes();
	SpawnAI();
}

void ATFDGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	// 팀 비율 결정
	TArray<APlayerState*> PlayerArray = GetGameState()->PlayerArray;
	int32 PlayerCnt = PlayerArray.Num();
	int32 PoliceCnt = UInGameUtility::GetPoliceRoleCount(PlayerCnt);

	// 배열을 랜덤으로 섞기
	for (int32 i = PlayerArray.Num() - 1; i >= 0; --i)
	{
		int32 SwapIndex = FMath::RandRange(0, i);
		PlayerArray.Swap(i, SwapIndex);
	}

	int32 AssignedPolice = 0;

	for (APlayerState* PState : PlayerArray)
	{
		if (!PState) continue;

		ATFDPlayerState* State = Cast<ATFDPlayerState>(PState);
		if (!State) continue;

		// 랜덤으로 경찰 역할 부여
		if (AssignedPolice < PoliceCnt)
		{
			State->SetTeamTag(TAG_Team_Cop);
			AssignedPolice++;
		}
		else
		{
			State->SetTeamTag(TAG_Team_Thief);
		}
	}
	GamePause(true);
}

void ATFDGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	APlayerController* PC = Cast<APlayerController>(C);
	if (!PC || !PC->Player)
	{
		return ;
	}
	ATFDPlayerState* PlayerState = PC->GetPlayerState<ATFDPlayerState>(); 
	if(GetGameState() && PlayerState)
	{
		GetGameState()->MarkPlayerReady(PlayerState);

		// 모든 플레이어가 접속했는지 확인 (GameState의 PlayerArray 사용)
		if(NumTravellingPlayers == 0 && NumPlayers == GetGameState()->PlayerArray.Num())
		{
			UE_LOG(LogTemp, Log, TEXT("All players are ready. Starting the game."));
			GamePause(false);
			GetGameState()->SetGameState(EGameState::Playing);
			StartPlay();
		}
	}
}

void ATFDGameMode::SpawnAI()
{
	FVector SpawnLoc = GetRandomPointInSpawnArea();
	FRotator SpawnRot = FRotator::ZeroRotator;

	FGameplayTag AITag =TAG_Team_Neutral;
	TSubclassOf<ATFDCharacterBase> AIClass = GetGameState()->GetRuleData().PawnClassAI;
	for (int32 i = 0; i < NumberOfAI; ++i)
	{
		SpawnLoc = GetRandomPointInSpawnAreaTag(AITag);
		UE_LOG(LogTemp, Display, TEXT("Spawning Player X:%.f,Y:%.f,Z:%.f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
		GetWorld()->SpawnActor(AIClass, &SpawnLoc, &SpawnRot);
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




void ATFDGameMode::OnCatchThief(APawn* Pawn)
{
	APlayerState* CatchPlayerState = Pawn->GetPlayerState();

	if (CatchPlayerState == nullptr)
		return;
	
	if (ATFDPlayerState* PS = Cast<ATFDPlayerState>(CatchPlayerState))
	{
		// ATFDPlayerState*로 WeakPtr 생성
		TWeakObjectPtr<ATFDPlayerState> WeakPS = MakeWeakObjectPtr<ATFDPlayerState>(PS);

		// 배열에 저장 가능
		GetGameState()->CaughtThiefPlayerStateArray.Add(WeakPS);
	}
	

	if (GetGameState()->CaughtThiefPlayerStateArray.Num() == GetGameState()->ThiefPlayerStateArray.Num())
	{
		GameEnd(EGameCompleteType::CatchAllThief);
	}
}

void ATFDGameMode::GameEnd(EGameCompleteType CompleteType)
{
	GetGameState()->SetGameState(EGameState::Result);
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

ATFDGameState* ATFDGameMode::GetGameState()
{
	if (GameState == nullptr)
	{
		GameState = Super::GetGameState<ATFDGameState>();
	}
	return GameState;
}


void ATFDGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float GameTime = GetGameState()->GetCurrentGameTimeSec();
	// 시간이 종료될 시 게임 종료 로직
	if (GameTime > GetGameState()->GetRuleData().PlayTimeSec)
	{
		GameEnd(EGameCompleteType::TimeLimit);
	}
}



