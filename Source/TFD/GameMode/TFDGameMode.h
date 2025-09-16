// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameState/TFDGameState.h"
#include "GameData/EGameEnums.h"
#include "TFDGameMode.generated.h"
/**
 * 
 */
class ATFDAICharacter;
class ATFDCharacter;
class ATFDSpawnVolume;
struct FGameplayTag;

UCLASS()
class TFD_API ATFDGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATFDGameMode();

	void OnCatchThief(APawn* Pawn);
	// 게임 종료시 처리될 내용이 담김.
	void GameEnd(EGameCompleteType CompleteType);
	ATFDGameState* GetGameState();
	void GamePause(bool bIsPaused);

	// 팀 배정 함수
	void AssignTeams();

#pragma region 

	//StartMatch()가 호출됐을 때 WaitingToStart 에서 InProgress 로 넘어가기 전에 넘어가도 되는지 판단(bool 값)
		virtual bool ReadyToStartMatch_Implementation() override;

	//MatchState 가 WaitingToStart로 바뀔 때 호출
	virtual void HandleMatchIsWaitingToStart() override;

	//StartMatch()가 성공해서 MatchState::InProgress 로 진입할 때 호출
	virtual void HandleMatchHasStarted() override;

	//EndMatch()가 호출되어 MatchState::WaitingPostMatch 로 바뀔 때 실행
	virtual void HandleMatchHasEnded() override;
#pragma endregion

	// SeamlessTravel 관련
	virtual void PostSeamlessTravel() override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
    
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	ATFDGameState* GameState;

	UFUNCTION()
	void HandleThiefScoreChanged(int32 NewScore);
	
public:
	void SpawnAI();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 NumberOfAI;
private:
	FVector GetRandomPointInSpawnArea();
	FVector GetRandomPointInSpawnAreaTag(FGameplayTag InTag);
	FVector GetRandomPointInSpawnAreaAI();

	ATFDSpawnVolume* GetRandomSpawnVolume();
	ATFDSpawnVolume* GetRandomSpawnVolumeTag(FGameplayTag InTag);

	void InitializeSpawnVolumes();
	void MovePlayerToRandomSpawnPoint(APlayerController* PlayerController);

	// 모든 플레이어의 PlayerState와 팀 선호 정보를 수집하는 함수
	void GatherPreferredTeams(TArray<ATFDPlayerState*>& OutPlayers, TArray<FGameplayTag>& OutPreferredTeams);
	
};
