// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TFDNativeGameplayTags.h"

#include "Blueprint/UserWidget.h"  // OutGame 관련 - Lobby UI 위젯 관련 헤더
#include "GameData/EGameEnums.h"
#include "TFDPlayerController.generated.h"


struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UPlayingWidget;
class UResultWidget;
class UHUDLayoutWidget;
class UMiniMapWidget;
class UReleaseWidget;

// Delegate 선언: 공인 IP가 준비되었을 때 알려주는 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPublicIPReady, const FString&, PublicIP);

/**
 * 
 */
UCLASS()
class TFD_API ATFDPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATFDPlayerController();

	UFUNCTION(BlueprintCallable)
	void SetMovemnetWalking(bool bMovement);

protected:
	virtual void BeginPlay() override; // OutGame 관련 추가
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AcknowledgePossession(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input|Default")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input|Default")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input|Default")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input|Default")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input")
	TObjectPtr<UInputAction> DashAction;

	// 스킬 시스템 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Input|Skill")
	TObjectPtr<UInputAction> Skill1Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Input|Skill")
	TObjectPtr<UInputAction> Skill2Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TFD|Input|Skill")
	TObjectPtr<UInputAction> Skill3Action;
	
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input")
	TObjectPtr<UInputAction> PauseAction;
	*/

private:
	void Dash(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumping();

	//===================================================
	// 스킬 시스템 관련

	// 입력 바인딩 함수
	void OnSkillInput1(const FInputActionValue& Value);
	void OnSkillInput2(const FInputActionValue& Value);
	void OnSkillInput3(const FInputActionValue& Value);

	// Skill input 공통 처리 함수
	void HandleSkillInput(int32 SlotIndex);
	//===================================================

	/*
	void Attack(const FInputActionValue& Value);
	void TogglePause(const FInputActionValue& Value);
	*/

	TWeakObjectPtr<UInputMappingContext> ActiveJobIMC; //OnUnPossess시 삭제할 직업 IMC 저장공간
	TArray<int32> JobBindingHandles; //OnUnPossess시 삭제할 직업 InputAction 리스트 저장공간
	void JobAbility(const FInputActionValue& Value, FGameplayTag InputTag); //DataAsset에서 추가한 액션과 태그로 자동 바인딩

//===================================================
// 이하 OutGame 관련 - Lobby
//===================================================
public:
	//virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void EnterLobby();
	// 이 플레이어가 호스트인지 확인
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsHostPlayer() const;

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	void RemoveLobbyUI();

	// 클라이언트에서 선호 팀을 서버에 요청하는 함수
	UFUNCTION(BlueprintCallable, Category = "Lobby|TeamSelection")
	void SendPreferredTeam(FGameplayTag PreferredTeam);

	// 서버에서 선호 팀을 PlayerState에 적용하는 RPC 함수
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Lobby|TeamSelection")
	void ServerSetPreferredTeam(const FGameplayTag& TeamTag);

public:
	UFUNCTION()
	FString GetLocalIP() const;

	// 요청 시도 함수
	UFUNCTION(BlueprintCallable, Category = "Network")
	void RequestPublicIP();

	// 저장된 IP를 반환하는 getter
	UFUNCTION(BlueprintCallable, Category = "Network")
	FString GetPublicIP() const;

public:
	// 공인 IP 저장용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FString CachedPublicIP;

	// Delegate 인스턴스
	UPROPERTY(BlueprintAssignable, Category = "Network")
	FOnPublicIPReady OnPublicIPReady;

	UFUNCTION()
	void HandleMatchInProgress();

	UFUNCTION()
	void HandleMatchWaitingPostMatch(FGameplayTag WinTeamTag, EGameCompleteType CompleteType);

	UFUNCTION()
	void HandleShowReleaseWidget();

	UFUNCTION()
	void HandleRemoveReleaseWidget();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LobbyWidgetInstance;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHUDLayoutWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayingWidget> PlayingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UResultWidget> ResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMiniMapWidget> MiniMapWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReleaseWidget> ReleaseWidgetClass;
};
