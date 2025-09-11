#pragma once

#include "CoreMinimal.h"
#include "Character/TFDCharacterBase.h"
#include "FGameRuleData.generated.h"


USTRUCT(BlueprintType)
struct FGameRuleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayTimeSec = 90.f;
	
	// 캐릭터 액터 클래스
	TSubclassOf<ATFDCharacterBase> PawnClassPolice;
	TSubclassOf<ATFDCharacterBase> PawnClassThief;
	TSubclassOf<ATFDCharacterBase> PawnClassAI;
};
