#pragma once


/**
 * 게임 상황에 맞춘 UI 정책 분리
 */
UENUM(BlueprintType)
enum class EHUDLayout : uint8
{
	// 타이틀과 로비
	OutGame,
	// 플레이어가 일반적으로 게임을 플레이할 때 (체력/스킬바/미니맵 등 표시)
	InGame,

	// 관전자 모드일 때 (플레이어 상태 UI 제거, 관전 전용 UI 표시)
	Spectator,

	// 메인 메뉴 화면 (로그인, 캐릭터 선택 등)
	MainMenu,

	// 게임 일시정지 메뉴
	PauseMenu
};

UENUM(BlueprintType)
enum class EUILayer : uint8
{
	// HUD의 기본 UI (체력, 스킬바, 크로스헤어 등)
	GameLayer,

	// 메뉴나 인벤토리 같은 전체 화면 UI
	MenuLayer,

	// 팝업/알림 (예: "레벨 업!" 메시지)
	PopupLayer,

	// 일시적 시스템 알림 (토스트, 데미지 플로팅 텍스트 등)
	NotificationLayer
};