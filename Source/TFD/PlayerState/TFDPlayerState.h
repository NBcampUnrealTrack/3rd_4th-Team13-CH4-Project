// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "GameData/EGameEnums.h"
#include "TFDPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNameChanged);

/**
 * 
 */
UCLASS()
class TFD_API ATFDPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	// DataAsset 설정 
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "GAS")
	FGameplayTag TeamTag;

public:
	void SetTeamTag(FGameplayTag Tag);
	FGameplayTag GetTeamTag() const;
	virtual void CopyProperties(APlayerState* NewPlayerState) override;
//===================================================
// 이하 OutGame 관련 - Lobby
// 플레이어 이름 복제, 변경시 델리게이트 호출
//===================================================
public:
	ATFDPlayerState();

	// 이름 변경 델리게이트 (클라이언트 UI 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "PlayerState")
	FOnPlayerNameChanged OnPlayerNameChanged;

	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& NewNickname);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FString GetNickname() const { return Nickname; }


protected:
	// 복제되는 플레이어 이름 변수
	UPROPERTY(ReplicatedUsing = OnRep_PlayerName)
	FString ReplicatedPlayerName;

	// 복제 완료 시 호출됨 (클라이언트)
	// MSB3073 오류 : 부모 클래스에서 이미 UFUNCTION으로 선언된 함수이므로 UFUNCTION 매크로 없이 override만 붙임
	//UFUNCTION()
	void OnRep_PlayerName();

	 UPROPERTY(ReplicatedUsing = OnRep_Nickname)
    FString Nickname;

    UFUNCTION()
    void OnRep_Nickname();


public:
	// 이름 설정 함수 (서버에서 호출)
	void SetPlayerName(const FString& NewName);

	// 네트워크 복제에 필요한 함수 재정의
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

// 플레이어가 선택한 선호 팀
UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
FGameplayTag PreferredTeam;
// 실제 배정된 팀 (복제 시 OnRep 호출)
UPROPERTY(ReplicatedUsing = OnRep_ActualTeam, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
FGameplayTag ActualTeam;

// 실제 팀 복제 시 호출 (클라이언트)
UFUNCTION()
void OnRep_ActualTeam();

// 접근자 함수
FGameplayTag GetPreferredTeam() const { return PreferredTeam; }
void SetPreferredTeam(const FGameplayTag& InTeamTag) { PreferredTeam = InTeamTag; }

FGameplayTag GetActualTeam() const { return ActualTeam; }
void SetActualTeam(const FGameplayTag& InTeam) { ActualTeam = InTeam; }


};