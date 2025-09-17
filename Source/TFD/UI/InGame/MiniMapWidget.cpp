// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/MiniMapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

    WorldMin = FVector2D(-6000.f, -7500.f);
    WorldMax = FVector2D(6000.f, 7500.f);

    if (MapImage)
    {
        MapSize = MapImage->Brush.ImageSize;

        // Brush.ImageSize가 0이면 CanvasSlot에서 실제 크기 가져오기
        if (MapSize.IsZero())
        {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MapImage->Slot))
            {
                MapSize = CanvasSlot->GetSize();
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("MapSize: X=%.2f, Y=%.2f"), MapSize.X, MapSize.Y);
}

void UMiniMapWidget::SetMapTexture(UTexture2D* NewTexture)
{
    if (MapImage && NewTexture)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(NewTexture);
        Brush.ImageSize = FVector2D(NewTexture->GetSizeX(), NewTexture->GetSizeY());
        MapImage->SetBrush(Brush);
    }
}

void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!OwnerPawn || !PlayerIcon) return;

    FVector Location = OwnerPawn->GetActorLocation();

    // X와 Y를 바꿔서 매핑
    float XRatio = (Location.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);
    float YRatio = (Location.X - WorldMin.X) / (WorldMax.X - WorldMin.X);

    FVector2D MiniMapPos = FVector2D(
        XRatio * MapSize.X,
        (1.f - YRatio) * MapSize.Y // Y축 반전 적용
    );

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
    {
        CanvasSlot->SetPosition(MiniMapPos);
    }
}

void UMiniMapWidget::SetOwnerPawn(APawn* InPawn)
{
    OwnerPawn = InPawn;

    if (OwnerPawn && PlayerIcon)
    {
        FVector PlayerLoc = OwnerPawn->GetActorLocation();

        float XRatio = (PlayerLoc.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y); // X/Y 뒤집음
        float YRatio = (PlayerLoc.X - WorldMin.X) / (WorldMax.X - WorldMin.X);

        FVector2D MiniMapPos = FVector2D(
            XRatio * MapSize.X,
            (1.f - YRatio) * MapSize.Y
        );

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
        {
            CanvasSlot->SetPosition(MiniMapPos);
        }

        // 🔹 로그 찍기
        UE_LOG(LogTemp, Warning, TEXT("[MiniMap] Player World: X=%.2f, Y=%.2f, MiniMap: X=%.2f, Y=%.2f"),
            PlayerLoc.X, PlayerLoc.Y, MiniMapPos.X, MiniMapPos.Y);
    }
}



