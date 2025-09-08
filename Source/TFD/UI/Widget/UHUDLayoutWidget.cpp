// Fill out your copyright notice in the Description page of Project Settings.


#include "UHUDLayoutWidget.h"


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"


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
	UCanvasPanel* Slot = GetLayerSlot(Layer);
	if (Slot && Widget)
	{
		Slot->AddChild(Widget);
	}
}

void UHUDLayoutWidget::RemoveFromLayer(EUILayer Layer, UUserWidget* Widget)
{
	UCanvasPanel* Slot = GetLayerSlot(Layer);
	if (Slot && Widget)
	{
		Slot->RemoveChild(Widget);
	}
}
