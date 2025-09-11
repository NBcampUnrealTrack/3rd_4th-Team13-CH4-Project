// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
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


	// AI 군중 배치 및 리플리케이트 
	void SpawnAICrowd();
	// 플레이어들 위치 배치와 연결
	void SetPlayerPositions();
	void OnCatchThief(APawn* APawn);
	// TODO: 도둑 폰으로 클래스 교체
	//  게임 시작 전, 결과 표시중 플레이어 조종 방지처리
	void GamePause();
	// 게임 종료시 처리될 내용이 담김.
	void OnHandleGameEnd(EGameCompleteType CompleteType);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostSeamlessTravel() override;

	//스폰위치 결정짓는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ATFDCharacter> CorpCharacterClass;

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
