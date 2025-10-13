#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UIResourceAsset.generated.h"

class UPaperSprite;

UENUM(BlueprintType)
enum class EUIIconType : uint8
{
	None          UMETA(DisplayName="None"),
	Cops          UMETA(DisplayName="Cops"),
	Thief         UMETA(DisplayName="Thief"),
	Gold          UMETA(DisplayName="Gold"),
	Clock         UMETA(DisplayName="Clock"),
	Jail          UMETA(DisplayName="Jail"),
	Exit          UMETA(DisplayName="Exit"),
	Handcuff      UMETA(DisplayName="Handcuff"),
	Invisibility  UMETA(DisplayName="Invisibility"),
	Teleport      UMETA(DisplayName="Teleport"),
	ThrowSlowItem UMETA(DisplayName="ThrowSlowItem"),
	SpeedUp		  UMETA(DisplayName = "SpeedUp")
};

UCLASS()
class UUIResourceAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Enum → Sprite 매핑 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EUIIconType, TSoftObjectPtr<UPaperSprite>> IconMap;

	// 헬퍼 함수: 아이콘 타입에 맞는 Sprite 반환
	void RequestIconAsync(EUIIconType IconType, TFunction<void(TObjectPtr<UPaperSprite>)> OnLoaded) const;

	static FSlateBrush MakeBrushFromSprite(UPaperSprite* Sprite, int32 Width, int32 Height);
};