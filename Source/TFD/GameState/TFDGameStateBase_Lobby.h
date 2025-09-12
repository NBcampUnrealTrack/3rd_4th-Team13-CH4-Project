// ATFDGameStateBase_Lobby.h
/*
	로비에 접속한 플레이어 리스트 관리
	PlayerState 추가/삭제 감지 -> 델리게이트로 UI 갱신 트리거
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TFDGameStateBase_Lobby.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListChanged);

UCLASS()
class TFD_API ATFDGameStateBase_Lobby : public AGameState
{
	GENERATED_BODY()
	
public:
	ATFDGameStateBase_Lobby();

	// 클라이언트용 접속자 리스트 갱신용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnPlayerListChanged OnPlayerListChanged;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetConnectedPlayerCount() const;

protected:
	virtual void HandleBeginPlay() override;

public:
	// 게임 상태에서 플레이어 상태가 추가되거나 제거될 때 호출됨
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	// 복제 설정 함수 추가
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
