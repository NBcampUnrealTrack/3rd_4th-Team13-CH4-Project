// Fill out your copyright notice in the Description page of Project Settings.


#include "UHUDLayoutWidget.h"
#include "Components/CanvasPanelSlot.h"

UCanvasPanel* UHUDLayoutWidget::GetLayerSlot(EUILayer Layer)
{
	UCanvasPanel* returnPanel = nullptr;
	switch (Layer)
	{
	case EUILayer::GameLayer:
		returnPanel = GameLayerSlot;
		break;
	case EUILayer::MenuLayer:
		returnPanel = MenuLayerSlot;
		break;
	case EUILayer::PopupLayer:
		returnPanel = PopupLayerSlot;
		break;
	case EUILayer::NotificationLayer:
		returnPanel = NotificationLayerSlot;
		break;
	default:;
	}
	return returnPanel;
}

void UHUDLayoutWidget::AddToLayer(EUILayer Layer, UUserWidget* Widget)
{
	UCanvasPanel* LayerSlot = GetLayerSlot(Layer);
	if (!LayerSlot || !Widget) return;

	// 캔버스에 위젯 추가
	UCanvasPanelSlot* CanvasSlot = LayerSlot->AddChildToCanvas(Widget);
	if (CanvasSlot)
	{
		// 부모 캔버스 기준으로 전체 화면에 맞춤
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(0.f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}
}

void UHUDLayoutWidget::RemoveFromLayer(EUILayer Layer, UUserWidget* Widget)
{
	UCanvasPanel* LayerSlot = GetLayerSlot(Layer);
	if (LayerSlot && Widget)
	{
		LayerSlot->RemoveChild(Widget);
	}
}
