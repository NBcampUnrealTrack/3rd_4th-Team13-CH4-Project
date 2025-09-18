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
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "Object/TFDSpawnVolume.h"
#include "Utility/InGameUtility.h"



ATFDGameMode::ATFDGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	DefaultPawnClass = nullptr;
	bUseSeamlessTravel = true;
}

void ATFDGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetGameState())
	{
		// 점수 변경 이벤트 구독
		GetGameState()->OnThiefScoreChanged.AddDynamic(this, &ATFDGameMode::HandleThiefScoreChanged);
	}
	// 게임 시작 시점에서 팀 배정
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
		{
			TFDPS->SetActualTeam(TFDPS->GetPreferredTeam());
		}
	}

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
	SpawnTransform.SetLocation(SpawnLocation);

	// SpawnActorDeferred로 Pawn 생성
	ATFDCharacterBase* Pawn = GetWorld()->SpawnActorDeferred<ATFDCharacterBase>(PawnClass, SpawnTransform, NewPlayer,
		nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!Pawn) return nullptr;

	const FGameRuleData& RuleData = GetGameState()->GetRuleData();
	ATFDPlayerState* PState = Cast<ATFDPlayerState>(NewPlayer->PlayerState);
	ATFDCharacterBase* TFDPawn = Cast<ATFDCharacterBase>(Pawn);
	if (PState && TFDPawn)
	{
		FGameplayTag Tag = PState->GetTeamTag();
		if (Tag == TAG_Team_Thief)
		{
			TFDPawn->CharacterData = RuleData.ThiefDataAsset;
			GetGameState()->ThiefPlayerStateArray.Add(PState);
		}
		else if (Tag == TAG_Team_Cop)
		{
			TFDPawn->CharacterData = RuleData.PoliceDataAsset;
			GetGameState()->PolicePlayerStateArray.Add(PState);
		}
	}
	// FinishSpawningActor 호출 (BeginPlay 직전)
	UGameplayStatics::FinishSpawningActor(TFDPawn, SpawnTransform);
	return Pawn;
}


bool ATFDGameMode::ReadyToStartMatch_Implementation()
{
	return MatchState == MatchState::InProgress;
}

void ATFDGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	InitializeSpawnVolumes();
	SpawnAI();
}

void ATFDGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasStarted"));
	//게임 시작 처리
	GetGameState()->GameRemainServerTime = GetGameState()->GetRuleData().PlayTimeSec;
	SetActorTickEnabled(true);
	GamePause(false);
}

void ATFDGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasEnded"));
	//게임 종료 처리


	SetActorTickEnabled(false);
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

	UE_LOG(LogTemp, Warning, TEXT("HandleSeamlessTravelPlayer"));
	APlayerController* PC = Cast<APlayerController>(C);
	if (!PC || !PC->Player)
	{
		return;
	}
	ATFDPlayerState* PlayerState = PC->GetPlayerState<ATFDPlayerState>();
	if (GetGameState() && PlayerState)
	{
		GetGameState()->MarkPlayerReady(PlayerState);

		// 모든 플레이어가 접속했는지 확인 (GameState의 PlayerArray 사용)
		if (NumTravellingPlayers == 0 && NumPlayers == GetGameState()->PlayerArray.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("All players are ready. Starting the game."));

			StartMatch(); //InProgress 상태로 전환

		}
	}
}

void ATFDGameMode::SpawnAI()
{
	FVector SpawnLoc = GetRandomPointInSpawnArea();
	FRotator SpawnRot = FRotator::ZeroRotator;

	FGameplayTag AITag = TAG_Team_Neutral;
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

		GetGameState()->OnThievesChanged.Broadcast();
	}


	if (GetGameState()->CaughtThiefPlayerStateArray.Num() == GetGameState()->ThiefPlayerStateArray.Num())
	{
		GameEnd(EGameCompleteType::CatchAllThief);
	}
}

void ATFDGameMode::HandleThiefScoreChanged(int32 NewScore)
{
	UE_LOG(LogTemp, Log, TEXT("Thief Score Changed: %d"), NewScore);

	//도둑 승리 점수 체크
	if (NewScore >= GetGameState()->GetRuleData().ThiefScoreForWin)
	{
		//일단은 게임 종료, 이후 점수 달성하면 특정 장소를 스폰할지 뭘할지 생각해보기
		GameEnd(EGameCompleteType::ThiefWinByScore);
	}
}

void ATFDGameMode::GameEnd(EGameCompleteType CompleteType)
{
	UE_LOG(LogTemp, Log, TEXT("Game End!!!!!!"));

	if (CompleteType == EGameCompleteType::TimeLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Police Win (TimeLimit)"));
		GetGameState()->SetWinTeam(TAG_Team_Cop, CompleteType);
	}
	else if (CompleteType == EGameCompleteType::CatchAllThief)
	{
		UE_LOG(LogTemp, Warning, TEXT("Police Win (CatchAllThief)"));
		GetGameState()->SetWinTeam(TAG_Team_Cop, CompleteType);
	}
	else if (CompleteType == EGameCompleteType::ThiefWinByScore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Thief Win (ThiefWinByScore)"));
		GetGameState()->SetWinTeam(TAG_Team_Thief, CompleteType);
	}

	EndMatch();
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
		if (ATFDAICharacter* WorldAICharacter = Cast<ATFDAICharacter>(AI->GetCharacter()))
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

	GetGameState()->GameRemainServerTime -= DeltaTime;

	// 호스트용 직접 브로드캐스트
	if (GetGameState()->HasAuthority())
	{
		GetGameState()->OnGameTimeChanged.Broadcast(GetGameState()->GameRemainServerTime);
	}


	// 시간이 종료될 시 게임 종료 로직
	if (GetGameState()->GameRemainServerTime < 0)
	{
		GameEnd(EGameCompleteType::TimeLimit);
	}
}


// 모든 플레이어의 PlayerState와 팀 선호 정보를 수집하는 함수
// 로비에서 팀 배정시 사용

void ATFDGameMode::GatherPreferredTeams(TArray<ATFDPlayerState*>& OutPlayers, TArray<FGameplayTag>& OutPreferredTeams)
{
	OutPlayers.Empty();
	OutPreferredTeams.Empty();

	if (!GetWorld())
		return;

	for (APlayerState* PS : GetGameState()->PlayerArray)
	{
		ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS);
		if (TFDPS)
		{
			OutPlayers.Add(TFDPS);
			OutPreferredTeams.Add(TFDPS->GetPreferredTeam());
		}
	}
}

// 팀 배정 함수
// 로비에서 모든 플레이어가 접속한 후에 호출

void ATFDGameMode::AssignTeams()
{
	UE_LOG(LogTemp, Log, TEXT("AssignTeams called"));
	TArray<ATFDPlayerState*> AllPlayers;
	TArray<FGameplayTag> PreferredTeams;
	// 모든 플레이어의 PlayerState와 선호 팀 수집
	GatherPreferredTeams(AllPlayers, PreferredTeams);
	const int32 PoliceTeamMax = 1; // 경찰 정원 설정 예시
	TArray<ATFDPlayerState*> PreferredPolicePlayers;
	TArray<ATFDPlayerState*> OtherPlayers;
	// 선호 경찰과 기타 선호자 분류
	for (int32 i = 0; i < PreferredTeams.Num(); ++i)
	{
		if (PreferredTeams[i] == TAG_Team_Cop)
		{
			PreferredPolicePlayers.Add(AllPlayers[i]);
		}
		else
		{
			OtherPlayers.Add(AllPlayers[i]);
		}
	}
	// 경찰선호자 수가 정원 이하일 경우 모두 경찰 배정
	if (PreferredPolicePlayers.Num() <= PoliceTeamMax)
	{
		for (ATFDPlayerState* Player : PreferredPolicePlayers)
		{
			Player->SetActualTeam(TAG_Team_Cop);
		}
		// 나머지 플레이어는 도둑 배정
		for (ATFDPlayerState* Player : OtherPlayers)
		{
			Player->SetActualTeam(TAG_Team_Thief);
		}
	}
	else
	{
		// 선호 경찰자들을 랜덤 섞기
		for (int32 i = PreferredPolicePlayers.Num() - 1; i > 0; --i)
		{
			int32 SwapIndex = FMath::RandRange(0, i);
			PreferredPolicePlayers.Swap(i, SwapIndex);
		}
		// 경찰 정원만큼 배정
		for (int32 i = 0; i < PoliceTeamMax; ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Cop);
		}
		// 초과자 및 기타 플레이어는 도둑 배정
		for (int32 i = PoliceTeamMax; i < PreferredPolicePlayers.Num(); ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Thief);
		}
		for (ATFDPlayerState* Player : OtherPlayers)
		{
			Player->SetActualTeam(TAG_Team_Thief);
		}
	}
	// 최종 배정 로그 출력 (수정된 부분)
	for (ATFDPlayerState* Player : AllPlayers)
	{
		UE_LOG(LogTemp, Log, TEXT("Player %s assigned to team %s"),
			*Player->GetPlayerName(),
			*(Player->GetActualTeam().ToString())

		);
	}

}

void ATFDGameMode::AssignTeamsOnGameStart()
{
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
		{
			TFDPS->SetActualTeam(TFDPS->GetPreferredTeam());
			UE_LOG(LogTemp, Log, TEXT("Player %s assigned ActualTeam: %s"),
				*TFDPS->GetPlayerName(),
				*TFDPS->GetActualTeam().GetTagName().ToString());
		}
	}
}

// PostSeamlessTravel 재정의
// 모든 플레이어가 접속한 후에 팀 배정 함수 호출
// 기존 PostSeamlessTravel 로직도 유지
