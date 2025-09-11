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
	void PlayerIsReady(AController* PlayerController);
	void GamePause(bool bIsPaused);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnPostLogin(AController* NewPlayer) override;
	//스폰위치 결정짓는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void PostSeamlessTravel() override;

protected:
	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	// 헤더 파일(.h)에 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ATFDCharacter> CorpCharacterClass;

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
