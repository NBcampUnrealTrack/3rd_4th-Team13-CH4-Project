#pragma once

#include "CoreMinimal.h"                                                                           
#include "PlayerState/TFDPlayerState.h"
#include "GameFramework/GameState.h"
#include "GameData/FGameRuleData.h"   
#include "TFDGameState.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float GetCurrentGameTimeSec() const;		// 현재 서버시간 - 게임 시작 서버시간 

	EGameState GetCurrentGameState() const;
	void SetGameState(EGameState NewState);

	void MarkPlayerReady(ATFDPlayerState* PS);
	int32 GetReadyPlayerCount() const;

	const FGameRuleData& GetRuleData() const;
	

	const int MinimumPlayerNum = 2;

	// HACK: 캡슐화 하기 번거로워서 공개 변수로 둠.
	// 도둑정보 Controller 리스트
	TArray<TWeakObjectPtr< ATFDPlayerState>> PolicePlayerStateArray;
	// 경찰정보 Controller 리스트
	TArray<TWeakObjectPtr< ATFDPlayerState>> ThiefPlayerStateArray;

	TArray<TWeakObjectPtr<ATFDPlayerState>> CaughtThiefPlayerStateArray;
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess));
	FGameRuleData GameRuleData;		//TODO: DA 로 변경

	UPROPERTY(Replicated)
	float GameStartServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_GameStateChange, BlueprintReadOnly)
	EGameState GameState;	// 게임상태 변경처리 OnRep 함수 생성하여 후처리
	
	TArray<ATFDPlayerState*> ReadyPlayers;

	// 게임 상태 변환을 알림
	UFUNCTION()
	void OnRep_GameStateChange();
};
