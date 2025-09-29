// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TFDGameMode.h"

#include "GameInstance/TFDGameInstance.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "TFDNativeGameplayTags.h"
#include "Character/TFDAICharacter.h"
#include "Character/TFDCharacter.h"
#include "Controller/TFDPlayerController.h"
#include "GameState/TFDGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "Object/AllowTeamTag.h"
#include "Object/TFDSpawnVolume.h"
#include "Object/TFDSpawnpoint.h"
#include "Utility/InGameUtility.h"
#include "Constants/TFDGameConstants.h"
#include "Utility/TFDBGMSubsystem.h"


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


	if (UWorld* World = GetWorld())
	{
		if (UTFDGameInstance* GI = Cast<UTFDGameInstance>(World->GetGameInstance()))
		{
			FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(this);
			if (UTFDBGMSubsystem* BGMSub = GI->GetSubsystem<UTFDBGMSubsystem>())
			{
				BGMSub->OnLevelChanged(CurrentLevel); // 첫 맵 재생
			}
		}
	}

	if (GetGameState())
	{
		// 점수 변경 이벤트 구독
		GetGameState()->OnThiefScoreChanged.AddDynamic(this, &ATFDGameMode::HandleThiefScoreChanged);
	}
	RuleData = GetGameState()->GetRuleData();
}

void ATFDGameMode::AssignTeams()
{
	// 모든 플레이어의 PlayerState와 선호팀 정보 수집
	TArray<ATFDPlayerState*> AllPlayers;
	TArray<FGameplayTag> PreferredTeams;

	GatherPreferredTeams(AllPlayers, PreferredTeams);

	const int32 PoliceTeamMax = UInGameUtility::GetPoliceRoleCount(AllPlayers.Num());

	TArray<ATFDPlayerState*> PreferredPolicePlayers;
	TArray<ATFDPlayerState*> OtherPlayers;

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

	TArray<ATFDPlayerState*> AssignedPolice;

	// 0명 -> 랜덤
	if (PreferredPolicePlayers.Num() == 0)
	{
		int32 NumToAssign = FMath::Min(PoliceTeamMax, OtherPlayers.Num());
		for (int32 i = 0; i < NumToAssign; ++i)
		{
			int32 RandomIndex = FMath::RandRange(0, OtherPlayers.Num() - 1);
			AssignedPolice.Add(OtherPlayers[RandomIndex]);
			OtherPlayers[RandomIndex]->SetActualTeam(TAG_Team_Cop);
			OtherPlayers.RemoveAt(RandomIndex);
		}
	}
	else if (PreferredPolicePlayers.Num() <= PoliceTeamMax)
	{
		for (ATFDPlayerState* Player : PreferredPolicePlayers)
		{
			Player->SetActualTeam(TAG_Team_Cop);
			AssignedPolice.Add(Player);
		}
	}
	else
	{
		for (int32 i = PreferredPolicePlayers.Num() - 1; i > 0; --i)
		{
			int32 SwapIndex = FMath::RandRange(0, i);
			PreferredPolicePlayers.Swap(i, SwapIndex);
		}
		for (int32 i = 0; i < PoliceTeamMax; ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Cop);
			AssignedPolice.Add(PreferredPolicePlayers[i]);
		}
		for (int32 i = PoliceTeamMax; i < PreferredPolicePlayers.Num(); ++i)
		{
			PreferredPolicePlayers[i]->SetActualTeam(TAG_Team_Thief);
			OtherPlayers.Add(PreferredPolicePlayers[i]);
		}
	}

	for (ATFDPlayerState* Player : OtherPlayers)
	{
		Player->SetActualTeam(TAG_Team_Thief);
	}

	for (ATFDPlayerState* Player : AllPlayers)
	{
		UE_LOG(LogTemp, Log, TEXT("Player %s assigned to team %s"),
		       *Player->GetPlayerName(),
		       *Player->GetActualTeam().ToString());
	}
}

void ATFDGameMode::GatherPreferredTeams(TArray<ATFDPlayerState*>& OutPlayers, TArray<FGameplayTag>& OutPreferredTeams)
{
	OutPlayers.Empty();
	OutPreferredTeams.Empty();

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

void ATFDGameMode::AssignTeamsOnGameStart()
{
	for (APlayerState* PS : GetGameState()->PlayerArray)
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

APawn* ATFDGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	if (!NewPlayer || !StartSpot) return nullptr;

	ATFDPlayerState* PS = NewPlayer->GetPlayerState<ATFDPlayerState>();

	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn failed: PlayerState is null"));
		return nullptr;
	}

	if (PS->GetActualTeam() == FGameplayTag::EmptyTag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn blocked: ActualTeam is empty for player %s"), *PS->GetPlayerName());
		return nullptr;
	}


	// Pawn 클래스 가져오기
	TSubclassOf<APawn> PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass) return nullptr;

	// SpawnTransform 설정
	FTransform SpawnTransform = StartSpot->GetActorTransform();
	FVector SpawnLocation = GetRandomPointInSpawnAreaTag(PS->GetActualTeam());
	SpawnTransform.SetLocation(SpawnLocation);

	// SpawnActorDeferred로 Pawn 생성
	ATFDCharacterBase* Pawn = GetWorld()->SpawnActorDeferred<ATFDCharacterBase>(PawnClass, SpawnTransform, NewPlayer,
		nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!Pawn) return nullptr;

	ATFDPlayerState* PState = Cast<ATFDPlayerState>(NewPlayer->PlayerState);
	ATFDCharacterBase* TFDPawn = Cast<ATFDCharacterBase>(Pawn);
	if (PState && TFDPawn)
	{
		FGameplayTag Tag = PState->GetActualTeam();
		if (Tag == TAG_Team_Thief)
		{
			TFDPawn->CharacterData = RuleData->ThiefDataAsset;
			GetGameState()->ThiefPlayerStateArray.Add(PState);
		}
		else if (Tag == TAG_Team_Cop)
		{
			TFDPawn->CharacterData = RuleData->PoliceDataAsset;
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
	SpawnItemStart();
}

void ATFDGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasStarted"));
	//게임 시작 처리
	GetGameState()->GameRemainServerTime = RuleData->PlayTimeSec;
	SetActorTickEnabled(true);
	GamePause(false);
}

void ATFDGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasEnded"));
	//게임 종료 처리


	SetActorTickEnabled(false);

	GetWorldTimerManager().SetTimer(
		LobbyReturnTimerHandle,
		this,
		&ATFDGameMode::ReturnToLobby,
		RuleData->ReturnToLobbySec,
		false
	);
}

void ATFDGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UE_LOG(LogTemp, Log, TEXT("PostSeamlessTravel called - Assigning Teams"));

	// 기존 랜덤 배정 로직 삭제함

	// 선호 팀 기반 배정 함수 호출
	AssignTeams();
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
	if (SpawnVolumes.Num() == 0)
		return;

	FGameplayTag AITag = TAG_Team_Neutral;
	TArray<ATFDSpawnVolume*> AIVolumes;
	AIVolumes.Reserve(SpawnVolumes.Num());
	for (ATFDSpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (SpawnVolume && SpawnVolume->CheckTeamTag(AITag))
		{
			AIVolumes.Add(SpawnVolume);
		}
	}

	if (AIVolumes.Num() < 1)
		return;

	FVector SpawnLoc = GetRandomPointInSpawnArea();
	FRotator SpawnRot = FRotator::ZeroRotator;


	TSubclassOf<ATFDCharacterBase> AIClass = RuleData->PawnClassAI;

	for (ATFDSpawnVolume* SpawnVolume : AIVolumes)
	{
		for (int32 i = 0; i <= SpawnVolume->SpawnNum; i++)
		{
			SpawnLoc = SpawnVolume->GetRandomPointInVolumeLineTrace();
			SpawnRot = FRotator(0.f, FMath::FRandRange(-180.f, 180.f), 0.f);
			//UE_LOG(LogTemp, Display, TEXT("Spawning Player X:%.f,Y:%.f,Z:%.f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
			GetWorld()->SpawnActor(AIClass, &SpawnLoc, &SpawnRot);
		}
	}
}

void ATFDGameMode::SpawnItemStart()
{
	TArray<AActor*> FoundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATFDSpawnpoint::StaticClass(), FoundSpawnPoints);

	for (AActor* Actor : FoundSpawnPoints)
	{
		if (ATFDSpawnpoint* SpawnPoint = Cast<ATFDSpawnpoint>(Actor))
		{
			// 예시: 시작 시 전부 스폰
			SpawnPoint->SpawnItem();
		}
	}
}


UDataTable* ATFDGameMode::GetDTAllowedTeamTag()
{
	return DTAllowedTeamTag;
}

FGameplayTagContainer ATFDGameMode::GetDTAllowedTeamTagContainer(FGameplayTag ArgGameplayTag)
{
	FGameplayTagContainer ResultContainer;

	if (!DTAllowedTeamTag)
	{
		UE_LOG(LogTemp, Warning, TEXT("AllowTeamDataTable is not set."));
		return ResultContainer;
	}

	TArray<FAllowTeamTag*> AllRows;
	DTAllowedTeamTag->GetAllRows<FAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->ItemTag == ArgGameplayTag)
		{
			ResultContainer = Row->AllowedTeamTag;
			break;
		}
	}

	if (ResultContainer.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No matching ItemTag found for: %s"), *ArgGameplayTag.ToString());
	}

	return ResultContainer;
}

TSubclassOf<AActor> ATFDGameMode::GetDTAllowedTeamTag_Item(FGameplayTag ArgGameplayTag)
{
	TSubclassOf<AActor> ResultItem;

	if (!DTAllowedTeamTag)
	{
		return ResultItem;
	}

	TArray<FAllowTeamTag*> AllRows;
	DTAllowedTeamTag->GetAllRows<FAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->ItemTag == ArgGameplayTag)
		{
			ResultItem = Row->ItemClass;
			break;
		}
	}

	return ResultItem;
}

float ATFDGameMode::GetDTAllowedTeamTag_Period(FGameplayTag ArgGameplayTag)
{
	float result = 0.f;

	if (!DTAllowedTeamTag)
	{
		return result;
	}

	TArray<FAllowTeamTag*> AllRows;
	DTAllowedTeamTag->GetAllRows<FAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->ItemTag == ArgGameplayTag)
		{
			result = Row->SpawnPeriodSec;
			break;
		}
	}

	return result;
}

FSpawnPointArray ATFDGameMode::GetSpawnPointArrayTag(ETeamType InEnum)
{
	if (FSpawnPointArray* FoundArray = WorldSpawnPointsByTeam.Find(InEnum))
	{
		return *FoundArray;
	}

	// 키가 존재하지 않을 경우 빈 배열 반환
	return FSpawnPointArray();
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
		if (!SpawnVolume)
		{
			UE_LOG(LogTemp, Warning, TEXT("No SpawnVolume found for tag: %s"), *InTag.ToString());
		}
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

void ATFDGameMode::InitializeSpawnPoints()
{
	if (WorldSpawnPointsByTeam.Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Spawn points already initialized."));
		return;
	}

	// 팀별 태그 맵 (팀 → FGameplayTag)
	TMap<ETeamType, FGameplayTag> TeamTagMap;
	TeamTagMap.Add(ETeamType::Cop, TAG_Team_Cop);
	TeamTagMap.Add(ETeamType::Thief, TAG_Team_Thief);
	TeamTagMap.Add(ETeamType::Neutral, TAG_Team_Neutral);

	// 모든 스폰포인트 순회
	for (TActorIterator<ATFDSpawnpoint> It(GetWorld()); It; ++It)
	{
		ATFDSpawnpoint* SpawnPoint = *It;
		if (!IsValid(SpawnPoint)) continue;

		for (auto& Pair : TeamTagMap)
		{
			ETeamType Team = Pair.Key;
			FGameplayTag TeamTag = Pair.Value;

			if (SpawnPoint->CheckItemTag(TeamTag))
			{
				WorldSpawnPointsByTeam.FindOrAdd(Team).Points.Add(SpawnPoint);
				break; // 한 팀에만 속한다고 가정
			}
		}
	}

	// 로그 출력
	for (auto& Pair : WorldSpawnPointsByTeam)
	{
		UE_LOG(LogTemp, Display, TEXT("%s: %d spawn points found."),
		       *UEnum::GetValueAsString(Pair.Key),
		       Pair.Value.Points.Num());
	}

	// FSpawnPointArray* CopSpawns = WorldSpawnPointsByTeam.Find(ETeamType::Cop);
	// if (CopSpawns && CopSpawns->Points.Num() > 0)
	// {
	// 	ATFDSpawnpoint* RandomSpawn = CopSpawns->Points[FMath::RandRange(0, CopSpawns->Points.Num() - 1)];
	// 	
	// }
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

void ATFDGameMode::OffCatchThief(APawn* Pawn)
{
	APlayerState* OffCatchPlayerState = Pawn->GetPlayerState();

	if (OffCatchPlayerState == nullptr)
		return;

	if (ATFDPlayerState* PS = Cast<ATFDPlayerState>(OffCatchPlayerState))
	{
		// ATFDPlayerState*로 WeakPtr 생성
		TWeakObjectPtr<ATFDPlayerState> WeakPS = MakeWeakObjectPtr<ATFDPlayerState>(PS);

		// 배열에 저장 가능
		GetGameState()->CaughtThiefPlayerStateArray.Remove(WeakPS);

		GetGameState()->OnThievesChanged.Broadcast();
	}
}

void ATFDGameMode::HandleThiefScoreChanged(int32 NewScore)
{
	UE_LOG(LogTemp, Log, TEXT("Thief Score Changed: %d"), NewScore);

	//도둑 승리 점수 체크
	if (NewScore >= RuleData->ThiefScoreForWin)
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

void ATFDGameMode::ReturnToLobby()
{
	GetWorldTimerManager().ClearTimer(LobbyReturnTimerHandle);

	FString TravelCmd = FString::Printf(TEXT("%s?listen"), TFDGameConstants::LobbyLevel);
	UE_LOG(LogTemp, Warning, TEXT("Returning to Lobby: %s"), *TravelCmd);

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TravelCmd, true); // SeamlessTravel
	}
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

	// 시간이 종료될 시 게임 종료 로직
	if (GetGameState()->GameRemainServerTime < 0)
	{
		GameEnd(EGameCompleteType::TimeLimit);
	}
}

void ATFDGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	UE_LOG(LogTemp, Warning, TEXT("Player Logout Exiting"));
	ATFDPlayerState* PS = Exiting->GetPlayerState<ATFDPlayerState>();
	if (!PS) return;

	ATFDGameState* GS = GetGameState();
	if (!GS) return;

	// 경찰/도둑/잡힌도둑 배열에서 제거, 각 배열이 변경되면 GameState에서 OnRep_로 UI 변경까지 진행됨
	GS->PolicePlayerStateArray.Remove(PS);
	GS->ThiefPlayerStateArray.Remove(PS);
	GS->CaughtThiefPlayerStateArray.Remove(PS);

	GS->OnThiefArrayChanged.Broadcast(GS->ThiefPlayerStateArray);

	CheckGameContinuable();
}

void ATFDGameMode::CheckGameContinuable()
{
	//게임이 지속 가능한지 체크
	ATFDGameState* GS = GetGameState();
	if (!GS) return;

	//도둑이 전부 나갔으면 게임 종료
	if (GS->ThiefPlayerStateArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No thieves left. Ending game."));
		GetGameState()->SetWinTeam(TAG_Team_Cop, EGameCompleteType::Aborted);
		EndMatch();
		return;
	}

	//도둑이 나간 경우 살아있는 도둑은 없고 잡힌 도둑만 남아있으면 게임 종료
	if (GS->ThiefPlayerStateArray.Num() == GS->CaughtThiefPlayerStateArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Alive thieves left. Ending game."));
		GetGameState()->SetWinTeam(TAG_Team_Cop, EGameCompleteType::Aborted);
		EndMatch();
		return;
	}

	//경찰이 전부 나갔으면 게임 종료
	if (GS->PolicePlayerStateArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No police left. Thieves win!"));
		GetGameState()->SetWinTeam(TAG_Team_Thief, EGameCompleteType::Aborted);
		EndMatch();
		return;
	}

	// 아직 게임 진행 가능 → 그냥 넘어감
	UE_LOG(LogTemp, Warning, TEXT("Game still playable. Continue..."));
}
