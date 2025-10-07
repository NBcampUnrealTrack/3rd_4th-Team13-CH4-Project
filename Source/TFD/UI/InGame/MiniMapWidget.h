// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapWidget.generated.h"

class UImage;
class UTexture2D;
class ATFDPlayerState;
class ATFDGameState;
class ATFDBaseObject;
/**
 * 
 */
UCLASS()
class TFD_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
public:

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void SetOwnerPawn(APawn* InPawn);

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void UpdateTeamPlayerStateArray(const TArray<TWeakObjectPtr<ATFDPlayerState>>& TeamArray);

    UFUNCTION()
    void UpdateTeamItemIcon(const TArray<TWeakObjectPtr<ATFDBaseObject>>& TeamItemArray);

    void TryInitializeOwnerPawnState();

    UPROPERTY(meta = (BindWidget))
    UImage* MapImage;

    UPROPERTY(meta = (BindWidget))
    UImage* PlayerIcon;

    UPROPERTY(BlueprintReadWrite, Category = "MiniMap")
    APawn* OwnerPawn;

    UPROPERTY(BlueprintReadWrite, Category = "MiniMap")
    ATFDPlayerState* OwnerPawnState;

    UPROPERTY()
    TArray<ATFDPlayerState*> TeamPlayerStateArray;

    UPROPERTY(BlueprintReadWrite, Category = "MiniMap")
    ATFDGameState* CurrentGameState;

    // 아이콘 위젯 배열
    UPROPERTY()
    TArray<UImage*> TeamPlayerIcons;

    // 팀원 아이콘 Texture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
    UTexture2D* TeamIconTexture;

    // 아이템 아이콘 배열
    UPROPERTY()
    TArray<UImage*> TeamItemIcons;

    // 아이템 아이콘 Texture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
    UTexture2D* TeamItemIconTexture;

    // 아이템 아이콘 Texture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
    UTexture2D* JailIconTexture;

    UPROPERTY(meta = (BindWidget))
    UImage* JailCellIcon;

    FVector2D WorldMin;
    FVector2D WorldMax;  
    FVector2D MapSize;   
};
