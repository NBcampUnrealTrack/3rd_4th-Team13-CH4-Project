// ATFDGameModeBase_Lobby.h
/*
	접속 처리
	PlayerState 이름 설정, GameState에 델리게이트 호출
*/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"  // UI 위젯 관련 헤더
#include "GameFramework/GameMode.h"
#include "TFDGameModeBase_Lobby.generated.h"

UCLASS()
class TFD_API ATFDGameModeBase_Lobby : public AGameMode
{
	GENERATED_BODY()
	
public:
	// 플레이어가 접속했을 때 호출되는 함수 오버라이드
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
