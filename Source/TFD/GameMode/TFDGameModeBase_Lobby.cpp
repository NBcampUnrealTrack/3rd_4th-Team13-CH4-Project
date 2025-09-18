// TFDGameModeBase_Lobby.cpp
#include "GameMode/TFDGameModeBase_Lobby.h"

#include "GameState/TFDGameStateBase_Lobby.h"
#include "Controller/TFDPlayerController_Title.h"
#include "Controller/TFDPlayerController.h"
#include "Constants/TFDGameConstants.h"
#include "GameFramework/PlayerState.h"

ATFDGameModeBase_Lobby::ATFDGameModeBase_Lobby()
{
	GameStateClass = ATFDGameStateBase_Lobby::StaticClass();
	bUseSeamlessTravel = true;
}

void ATFDGameModeBase_Lobby::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UE_LOG(LogTemp, Warning, TEXT("PostSeamlessTravel() - Lobby Level Entered (Server Only)"));

	if (ATFDGameStateBase_Lobby* GS = GetGameState<ATFDGameStateBase_Lobby>())
	{
		GS->StartLobby();
	}
}

void ATFDGameModeBase_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer) return;

	ATFDGameStateBase_Lobby* LobbyGS = GetGameState<ATFDGameStateBase_Lobby>();
	if (!LobbyGS) return;

	// 접속 인원 체크
	int32 CurrentCount = LobbyGS->GetConnectedPlayerCount();

	//===================================================
	// 최대 인원 초과 시 접속 거부
	//===================================================
	if (CurrentCount > TFDGameConstants::MaxPlayerCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[ATFDGameModeBase_Lobby][PostLogin] Player tried to join but MAX player count reached!"));

		// 클라이언트에게 접속 초과 알림 RPC 호출
		ATFDPlayerController_Title* TitlePC = Cast<ATFDPlayerController_Title>(NewPlayer);
		if (TitlePC)
		{
			TitlePC->ClientNotifyMaxPlayerReached();
		}

		return;
	}

	//===================================================
	// 접속 허용
	//===================================================
	/*
		1. PlayerState 이름 설정
		2. GameState에 델리게이트 호출
	*/
	if (NewPlayer->PlayerState)
	{
		// 이름 지정
		FString PlayerName = FString::Printf(TEXT("Player_%d"), NewPlayer->PlayerState->GetPlayerId());
		NewPlayer->PlayerState->SetPlayerName(PlayerName);
		UE_LOG(LogTemp, Warning, TEXT("[ATFDGameModeBase_Lobby][PostLogin] Player name set to: %s"), *PlayerName);

		// GameState의 델리게이트를 통해 접속자 리스트 갱신 알림
		LobbyGS->OnPlayerListChanged.Broadcast();
	}
}
