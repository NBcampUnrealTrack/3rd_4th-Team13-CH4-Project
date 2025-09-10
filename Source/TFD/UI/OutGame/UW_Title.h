// UW_Title.h
/*
	[2025-09-10] 양한아 최초 작성

	타이틀 화면 UI를 담당하는 위젯 클래스입니다.
	- 서버 생성, 서버 접속, 게임 종료 등의 기능을 가진 버튼들을 포함

	구성 위젯:
	- Btn_CreateServer: 리슨 서버 생성 및 로비로 진입
	- Btn_JoinServer: 서버 IP 입력 화면으로 이동
	- Btn_QuitGame: 게임 종료 (옵션)

	기능:
	- 버튼 클릭 이벤트를 NativeConstruct에서 바인딩하고, NativeDestruct에서 언바인딩
	- 각 버튼 클릭 시 실행되는 함수 정의 (OnCreateServerClicked, OnJoinServerClicked, OnQuitGameClicked)
*/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Title.generated.h"

class UButton;
class UEditableText;

UCLASS()
class TFD_API UUW_Title : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnCreateServerClicked();

	UFUNCTION()
	void OnJoinServerClicked();

	UFUNCTION()
	void OnQuitGameClicked();

protected:
	//===================================================
	// 바인딩
	//===================================================
	virtual void NativeConstruct() override;	// 버튼 바인딩
	virtual void NativeDestruct() override;		// 버튼 언바인딩

protected:
	//===================================================
	// 필수 버튼
	//===================================================
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_CreateServer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_JoinServer;

	//===================================================
	// 없으면 nullptr 버튼
	//===================================================
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_QuitGame;
};
