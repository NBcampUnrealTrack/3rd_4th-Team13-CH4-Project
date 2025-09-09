// Fill out your copyright notice in the Description page of Project Settings.


#include "UHUDLayoutWidget.h"


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
	if (LayerSlot && Widget)
	{
		LayerSlot->AddChild(Widget);
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
