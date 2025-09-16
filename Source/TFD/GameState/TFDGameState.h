#pragma once

#include "CoreMinimal.h"                                                                           
#include "PlayerState/TFDPlayerState.h"
#include "GameFramework/GameState.h"
#include "GameData/FGameRuleData.h"
#include "GameData/EGameEnums.h"
#include "TFDGameState.generated.h"



// 도둑 전체 점수 변경 이벤트 (UI나 GameMode에서 구독 가능)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThiefScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThievesChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameTimeChanged, float, RemainingTimeSec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMachInProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchWaitingPostMatch, FGameplayTag, WinTeamTag, EGameCompleteType, CompleteType);


UCLASS()
class TFD_API ATFDGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATFDGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float GetCurrentGameTimeSec() const;		// 현재 서버시간 - 게임 시작 서버시간 


	void MarkPlayerReady(ATFDPlayerState* PS);
	int32 GetReadyPlayerCount() const;

	const FGameRuleData& GetRuleData() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddThiefScore(int32 Points);

	UFUNCTION()
	void SetWinTeam(FGameplayTag WinTeam, EGameCompleteType InCompleteType);

	EGameCompleteType GetInCompleteType() const { return CompleteType; }
	FGameplayTag GetWinTeamTag() const { return WinTeamTag; }
protected:

	virtual void OnRep_MatchState() override;

	UFUNCTION()
	void OnRep_CaughtThiefPlayerStateArray();

	UFUNCTION()
	void OnRep_ThiefScore();

	UFUNCTION()
	void OnRep_GameRemainTime();
public:

	const int MinimumPlayerNum = 2;

	// HACK: 캡슐화 하기 번거로워서 공개 변수로 둠.
	// 도둑정보 Controller 리스트
	TArray<TWeakObjectPtr< ATFDPlayerState>> PolicePlayerStateArray;
	// 경찰정보 Controller 리스트
	UPROPERTY(Replicated)
	TArray<TWeakObjectPtr< ATFDPlayerState>> ThiefPlayerStateArray;
	// 잡힌 도둑 Controller 리스트
	UPROPERTY(ReplicatedUsing = OnRep_CaughtThiefPlayerStateArray)
	TArray<TWeakObjectPtr<ATFDPlayerState>> CaughtThiefPlayerStateArray;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnThiefScoreChanged OnThiefScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnThievesChanged OnThievesChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameTimeChanged OnGameTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMachInProgress OnMachInProgress;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMatchWaitingPostMatch OnMatchWaitingPostMatch;

	UPROPERTY(ReplicatedUsing = OnRep_GameRemainTime, BlueprintReadOnly, Category = "Time")
	float GameRemainServerTime = 5.f;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess));
	FGameRuleData GameRuleData;		//TODO: DA 로 변경

	TArray<ATFDPlayerState*> ReadyPlayers;

	// 현재 도둑 팀 점수 합계
	UPROPERTY(ReplicatedUsing = OnRep_ThiefScore, BlueprintReadOnly, Category = "Score")
	int32 ThiefTotalScore = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	EGameCompleteType CompleteType;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FGameplayTag WinTeamTag;
};
