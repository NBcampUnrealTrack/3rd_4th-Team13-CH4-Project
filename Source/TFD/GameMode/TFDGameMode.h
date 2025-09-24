// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameState/TFDGameState.h"
#include "GameData/EGameEnums.h"
#include "GameData/TFDGameRuleData.h"
#include "TFDGameMode.generated.h"

class ATFDAICharacter;
class ATFDCharacter;
class ATFDSpawnVolume;
class ATFDSpawnpoint;
struct FGameplayTag;


UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Cop UMETA(DisplayName = "Cop"),
	Thief UMETA(DisplayName = "Thief"),
	Neutral UMETA(DisplayName = "Neutral")
};

USTRUCT(BlueprintType)
struct FSpawnPointArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ATFDSpawnpoint*> Points;
};

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

	void AssignTeams();

	UFUNCTION(BlueprintCallable)
	void AssignTeamsOnGameStart();


#pragma region 게임 상태 변화에 따른 로직

	//StartMatch()가 호출됐을 때 WaitingToStart 에서 InProgress 로 넘어가기 전에 넘어가도 되는지 판단 (bool 값)
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
	virtual void Logout(AController* Exiting) override;

	//virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UPROPERTY()
	TArray<ATFDSpawnVolume*> SpawnVolumes;

	ATFDGameState* GameState;

	UTFDGameRuleData* RuleData;

	UFUNCTION()
	void HandleThiefScoreChanged(int32 NewScore);

	UFUNCTION()
	void CheckGameContinuable();

	FTimerHandle LobbyReturnTimerHandle;

	UFUNCTION()
	void ReturnToLobby();


#pragma region 스폰관련

protected:
	UPROPERTY()
	TMap<ETeamType, FSpawnPointArray> WorldSpawnPointsByTeam;

public:
	void SpawnAI();
	void SpawnItemStart();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 NumberOfAI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable")
	UDataTable* DTAllowedTeamTag;

public:
	UDataTable* GetDTAllowedTeamTag();
	FGameplayTagContainer GetDTAllowedTeamTagContainer(FGameplayTag ArgGameplayTag);
	TSubclassOf<AActor> GetDTAllowedTeamTag_Item(FGameplayTag ArgGameplayTag);
	float GetDTAllowedTeamTag_Period(FGameplayTag ArgGameplayTag);

private:
	//팀 enum 넣으면 SpawnPointArray 받는 함수
	FSpawnPointArray GetSpawnPointArrayTag(ETeamType InEnum);

	FVector GetRandomPointInSpawnArea();
	FVector GetRandomPointInSpawnAreaTag(FGameplayTag InTag);
	FVector GetRandomPointInSpawnAreaAI();
	
	ATFDSpawnVolume* GetRandomSpawnVolume();
	ATFDSpawnVolume* GetRandomSpawnVolumeTag(FGameplayTag InTag);

	void InitializeSpawnPoints();

	void InitializeSpawnVolumes();
	void MovePlayerToRandomSpawnPoint(APlayerController* PlayerController);

	// 선호 팀 수집 함수
	void GatherPreferredTeams(TArray<ATFDPlayerState*>& OutPlayers, TArray<FGameplayTag>& OutPreferredTeams);
	
#pragma endregion
};
