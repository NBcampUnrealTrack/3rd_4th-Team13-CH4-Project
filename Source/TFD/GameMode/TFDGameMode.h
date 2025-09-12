// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameState/TFDGameState.h"
#include "GameData/EGameEnums.h"
#include "TFDGameMode.generated.h"

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
	
};
