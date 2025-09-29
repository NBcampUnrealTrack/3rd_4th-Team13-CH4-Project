// UW_Lobby.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Lobby.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;

UCLASS()
class TFD_API UUW_Lobby : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdatePlayerList(const TArray<class APlayerState*>& PlayerStates);

protected:
	//===================================================
	// 바인딩
	//===================================================
	virtual void NativeConstruct() override;	// 버튼 바인딩 + 델리게이트 바인딩
	virtual void NativeDestruct() override;		// 버튼 언바인딩 + 델리게이트 언바인딩

	// Delegate 콜백 함수
	UFUNCTION()
	void OnPublicIPReceived(const FString& PublicIP);

private:
	// 버튼 이벤트 처리 함수
	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnLeaveClicked();

	// 접속자 리스트 델리게이트 처리 함수
	UFUNCTION()
	void HandlePlayerListChanged();

	// Host인지 여부에 따라 UI 제어
	void UpdateUIByRole();

	void InitHostIP();

private:
	//===================================================
	// 공통 (Host, 일반)
	//===================================================
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Leave;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> SB_PlayerList;

	//===================================================
	// Host 전용
	//===================================================
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_HostIP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_Play;

	// Host IP 문자열 저장용
	FString HostIP;

	FTimerHandle RefreshTimerHandle;
};