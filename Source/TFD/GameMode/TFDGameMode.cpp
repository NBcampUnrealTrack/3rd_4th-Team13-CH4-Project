// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TFDGameMode.h"


#include "EngineUtils.h"
#include "TFDNativeGameplayTags.h"
#include "Character/TFDCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "Object/TFDSpawnVolume.h"




void ATFDGameMode::BeginPlay()
{
	Super::BeginPlay();


	InitializeSpawnVolumes();
	SpawnPlayer(3);
}


void ATFDGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// InitializeSpawnVolumes();
	//
	// // 폰이 아직 안생긴 경우가 있을수있어 지연처리
	// if (APawn* ExistingPawn = NewPlayer->GetPawn())
	// {
	// 	FVector SpawnLocation = GetRandomPointInSpawnArea();
	// 	ExistingPawn->SetActorLocation(SpawnLocation);
	// 	UE_LOG(LogTemp, Display, TEXT("Pawn moved immediately"));
	// }
	// else
	// {
	// 	// Pawn이 없으면 기다렸다가 처리
	// 	UE_LOG(LogTemp, Warning, TEXT("Pawn not ready, waiting..."));
	//
	// 	GetWorldTimerManager().SetTimer(PlayerSpawnTimerHandle, [this, NewPlayer]()
	// 	{
	// 		MovePlayerToRandomSpawnPoint(NewPlayer);
	// 	}, 1.0f, false);
	// }
}

AActor* ATFDGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 스폰 볼륨이 초기화되지 않았다면 초기화
	if (SpawnVolumes.Num() == 0)
	{
		InitializeSpawnVolumes();
		UE_LOG(LogTemp, Display, TEXT("CPS_1"));
	}

	// 랜덤 위치를 얻어서 임시 액터 생성
	FVector RandomLocation = GetRandomPointInSpawnArea();


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

void ATFDGameMode::SpawnAI(int32 NumberOfPeople)
{
	FVector SpawnLoc = GetRandomPointInSpawnArea();
	FRotator SpawnRot = FRotator::ZeroRotator;

	// GetWorld()->SpawnActor<ATFDCharacter>(, SpawnLoc, SpawnRot);
}

void ATFDGameMode::SpawnPlayer(int32 NumberOfPeople)
{
	FVector SpawnLoc = FVector(0.f, 0.f, 0.f);
	FRotator SpawnRot = FRotator::ZeroRotator;

	for (int32 i = 0; i < NumberOfPeople; ++i)
	{
		SpawnLoc = GetRandomPointInSpawnArea();
		UE_LOG(LogTemp, Display, TEXT("Spawning Player X:%.f,Y:%.f,Z:%.f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
		GetWorld()->SpawnActor<ATFDCharacter>(CorpCharacterClass, SpawnLoc, SpawnRot);
	}
}

FVector ATFDGameMode::GetRandomPointInSpawnArea()
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

