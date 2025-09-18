// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Character/TFDCharacterBase.h"
#include "TFDGameRuleData.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UTFDGameRuleData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//게임 결과 이후 로비로 돌아가기 전 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	float ReturnToLobbySec = 5.f;

	//게임 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	float PlayTimeSec = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	int32 NumberOfAI = 10;

	//도둑팀 승리 조건 점수 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	int32 ThiefScoreForWin = 500;

	// 캐릭터 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	TSubclassOf<ATFDCharacterBase> PawnClassAI;

	//게임에서 사용할 경찰과 도둑의 데이터에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	UTFDPlayerDataAsset* PoliceDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	UTFDPlayerDataAsset* ThiefDataAsset;
};
