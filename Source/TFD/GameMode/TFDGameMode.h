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
	void SpawnAI();

	
	void OnCatchThief(APawn* APawn);
	// 게임 종료시 처리될 내용이 담김.
	void OnHandleGameEnd(EGameCompleteType CompleteType);
	void PlayerIsReady(AController* PlayerController);

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostSeamlessTravel() override;

	//스폰위치 결정짓는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	// 헤더 파일(.h)에 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ATFDCharacter> CorpCharacterClass;

	ATFDGameState* GameState;
	
public:
	void SpawnAI(int32 NumberOfPeople = 1);
	void SpawnPlayer(int32 NumberOfPeople = 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ATFDAICharacter> AICharacter;
	

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

private:
	FTimerHandle PlayerSpawnTimerHandle;
	
};
