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
	ATFDGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float GetCurrentGameTimeSec() const;		// 현재 서버시간 - 게임 시작 서버시간 


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

	TArray<ATFDPlayerState*> ReadyPlayers;

	virtual void OnRep_MatchState() override;

};
