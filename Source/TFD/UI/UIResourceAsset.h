#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/Image.h"
#include "UIResourceAsset.generated.h"

UCLASS()
class UUIResourceAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> IconCops;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> IconThief;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> IconGold;
};
