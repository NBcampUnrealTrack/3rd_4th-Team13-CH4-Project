// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/EUIDefine.h"
#include "UHUDLayoutWidget.generated.h"

/**
 * Layout 별 위젯을 보유하는 위젯
 */
UCLASS()
class TFD_API UHUDLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Layer 슬롯 맵 WBP에서 CanvasPanel을 BindWidget
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* GameLayerSlot;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* MenuLayerSlot;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* PopupLayerSlot;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* NotificationLayerSlot;

	UCanvasPanel* GetLayerSlot(::EUILayer Layer);
	// 레이어에 위젯 추가
	void AddToLayer(EUILayer Layer, UUserWidget* Widget);

	// 레이어에서 위젯 제거
	void RemoveFromLayer(EUILayer Layer, UUserWidget* Widget);
};
