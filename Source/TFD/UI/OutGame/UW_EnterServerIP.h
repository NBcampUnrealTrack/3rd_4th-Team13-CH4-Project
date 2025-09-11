// UW_EnterServerIP.h
/*
	[2025-09-10] 양한아 최초 작성

	서버에 접속하기 위한 IP 입력 위젯 클래스입니다.
	- 유저가 직접 IP 주소를 입력하고, 접속 버튼을 눌러 서버에 접속 시도
	- 뒤로 가기 버튼을 통해 타이틀 화면으로 돌아갈 수 있음

	구성 위젯:
	- Btn_ConnectServer: 입력된 IP로 접속 시도
	- Btn_Back: 이전 화면(타이틀)로 복귀
	- EditIP: 유저가 IP 주소를 입력하는 텍스트 박스

	기능:
	- 버튼 클릭 이벤트 바인딩 및 해제 (NativeConstruct / NativeDestruct)
	- 각각의 버튼 클릭 시 실행되는 함수 정의 (OnConnectClicked / OnBackClicked)
*/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_EnterServerIP.generated.h"

class UButton;
class UEditableTextBox;

UCLASS()
class TFD_API UUW_EnterServerIP : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnConnectClicked();

	UFUNCTION()
	void OnBackClicked();

protected:
	virtual void NativeConstruct() override;	// 버튼 바인딩
	virtual void NativeDestruct() override;		// 버튼 언바인딩

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_ConnectServer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Back;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditIP;

private:
	// 처음 편집했는지 체크하는 변수
	bool bIsFirstEdit = true;

	UFUNCTION()
	void OnEditIPTextChanged(const FText& Text);
};

