// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TFDGameMode.generated.h"

/**
 * 
 */
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


	//pre start match 
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void HandleMatchIsWaitingToStart() override;
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
