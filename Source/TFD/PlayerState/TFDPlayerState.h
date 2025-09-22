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

	//��ȣ �� (����/����) - Lobby ���� ����, GameState���� �� ������ Ȱ��
	UPROPERTY(Replicated)
	FGameplayTag PreferredTeam;

	//���� �� (����/����) - GameState���� �� ���� �� ����
	// ���� �� ���� �Ϸ� �� ó�� �Լ� ���
	UPROPERTY(ReplicatedUsing = OnRep_ActualTeam)
	FGameplayTag ActualTeam;

	void SetActualTeam(const FGameplayTag& TeamTag);
	FGameplayTag GetActualTeam() const;

	
protected:
	// 복제되는 플레이어 이름 변수
	UPROPERTY(ReplicatedUsing = OnRep_PlayerName)
	FString ReplicatedPlayerName;

	// 복제 완료 시 호출됨 (클라이언트)
	// MSB3073 오류 : 부모 클래스에서 이미 UFUNCTION으로 선언된 함수이므로 UFUNCTION 매크로 없이 override만 붙임
	//UFUNCTION()
	void OnRep_PlayerName();

	// ActualTeam ���� �Ϸ� �� ȣ��� (Ŭ���̾�Ʈ)
	UFUNCTION()
	void OnRep_ActualTeam();

public:
	// �̸� ���� �Լ� (�������� ȣ��)
	void SetPlayerName(const FString& NewName);

	// ��Ʈ��ũ ������ �ʿ��� �Լ� ������
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
