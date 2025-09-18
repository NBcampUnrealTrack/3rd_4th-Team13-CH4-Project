#pragma once

#include "CoreMinimal.h"
#include "Character/TFDCharacterBase.h"
#include "FGameRuleData.generated.h"


USTRUCT(BlueprintType)
struct FGameRuleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReturnToLobbySec = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayTimeSec = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ThiefScoreForWin = 500;
	
	// 캐릭터 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATFDCharacterBase> PawnClassAI;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTFDPlayerDataAsset* PoliceDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTFDPlayerDataAsset* ThiefDataAsset;
};
