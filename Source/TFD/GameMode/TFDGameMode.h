// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TFDGameMode.generated.h"

class ATFDCharacter;
class ATFDSpawnVolume;
/**
 * 
 */
UCLASS()
class TFD_API ATFDGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//스폰위치 결정짓는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

  
protected:
	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	// 헤더 파일(.h)에 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ATFDCharacter> CorpCharacterClass;
	
public:
	void SpawnAI(int32 NumberOfPeople = 1);
	void SpawnPlayer(int32 NumberOfPeople = 1);
private:
	FVector GetRandomPointInSpawnArea();
	void	InitializeSpawnVolumes();
	void	MovePlayerToRandomSpawnPoint(APlayerController* PlayerController);
private:
	FTimerHandle PlayerSpawnTimerHandle;
	
};


