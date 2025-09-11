// TFDPlayerController_Title.h
/*
	[2025-09-10] 양한아 최초 작성

	타이틀 화면에서 사용되는 PlayerController 클래스입니다.
	- 타이틀 관련 UI를 초기화하고 전환하는 역할을 수행
	- 서버 생성 또는 서버 접속 버튼 클릭 시 처리 로직 담당

	주요 기능:
	- BeginPlay에서 타이틀 UI 위젯 생성 및 표시
	- 버튼 이벤트와 연동된 CreateServer(), JoinServer() 함수로 서버 생성/접속 처리
	- Title UI <-> IP 입력 UI 전환 기능 제공

	멤버 변수:
	- TitleWidgetClass / EnterIPWidgetClass: 위젯 클래스 (에디터에서 할당)
	- TitleWidgetInstance / EnterIPWidgetInstance: 현재 화면에 표시될 UI 인스턴스
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Blueprint/UserWidget.h"  // UI 위젯 관련 헤더

#include "TFDPlayerController_Title.generated.h"

UCLASS()
class TFD_API ATFDPlayerController_Title : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	//===================================================
	// 버튼 바인딩용 함수
	//===================================================
	UFUNCTION()
	void CreateServer();

	UFUNCTION()
	void JoinServer(const FString& ServerIP);

	//===================================================
	// UI 전환 함수
	//===================================================
	void ShowTitleUI();
	void ShowEnterIPUI();

	//===================================================
	// 서버에서 호출할 클라이언트용 함수
	//===================================================
	UFUNCTION(Client, Reliable)
	void ClientNotifyMaxPlayerReached();

private:
	//===================================================
	// Title UI
	//===================================================
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TitleWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> TitleWidgetInstance;

	//===================================================
	// IP 입력 UI
	//===================================================
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnterIPWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> EnterIPWidgetInstance;

	//===================================================
	// 팝업
	//===================================================
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MaxPlayerPopupClass;

};
