// TFDGameConstants.h
#pragma once

#include "CoreMinimal.h"

class TFDGameConstants
{
public:
	//===== 레벨 경로 =====
	static constexpr const TCHAR* TitleLevel = TEXT("/Game/Maps/L_Title");
	static constexpr const TCHAR* LobbyLevel = TEXT("/Game/Maps/L_Lobby");
	static constexpr const TCHAR* GameLevel = TEXT("/Game/Cartoon_City_Free/Maps/Demonstration"); // 임시 게임 맵

	//===== 접속 인원 제한 =====
	static constexpr int32 MaxPlayerCount = 6;
	static constexpr int32 MinPlayerCount = 2;

	//===== 대기 시간 등 설정 =====
	static constexpr float GameStartCountdownTime = 15.0f; // 15초 대기 후 게임 시작
};