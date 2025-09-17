// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapWidget.generated.h"

class UImage;
class UTexture2D;
/**
 * 
 */
UCLASS()
class TFD_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // 맵 텍스처 설정 함수
    UFUNCTION(BlueprintCallable, Category = "MiniMap")
    void SetMapTexture(UTexture2D* NewTexture);

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void SetOwnerPawn(APawn* InPawn);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UImage* MapImage;

    UPROPERTY(meta = (BindWidget))
    UImage* PlayerIcon;

    UPROPERTY(BlueprintReadWrite, Category = "MiniMap")
    APawn* OwnerPawn;

    FVector2D WorldMin;
    FVector2D WorldMax;  
    FVector2D MapSize;   
};
