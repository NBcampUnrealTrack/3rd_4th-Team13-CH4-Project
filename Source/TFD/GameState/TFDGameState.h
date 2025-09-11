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
	float GetCurrentGameTimeSec();		// 현재 서버시간 - 게임 시작 서버시간 
	EGameState GetCurrentGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MarkPlayerReady(ATFDPlayerState* PS);
	int32 GetReadyPlayerCount() const;

	const int MinimumPlayerNum = 2;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess));
	FGameRuleData GameRuleData;		//TODO: DA 로 변경

	float GameStartServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_GameStateChange, BlueprintReadOnly)
	EGameState GameState;	// 게임상태 변경처리 OnRep 함수 생성하여 후처리
	
	// 현재 게임 상태필요
	// 도둑정보 Controller 리스트
	// 경찰정보 Controller 리스트

	UPROPERTY(Replicated)
	TArray<ATFDPlayerState*> ReadyPlayers;

	// 게임 시작 알림용 Multicast
	UFUNCTION()
	void OnRep_GameStateChange();
};
