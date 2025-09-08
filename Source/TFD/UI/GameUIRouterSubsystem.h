// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameUIRouterSubsystem.generated.h"

/**
 * UI 관리 서브 시스템
 */
UCLASS()
class TFD_API UGameUIRouterSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
private:
	// 현재 HUD Layout 추적
	TMap<EUILayer, UUserWidget*> ActiveWidgets;
	EHUDLayout CurrentLayout;
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void InitializeHUD();
	
	// HUD 레이아웃 변경 (예: MainHUD → SpectatorHUD)
	void PushLayout(EHUDLayout LayoutType);

	// HUD 레이아웃 제거 (뒤로가기)
	void PopLayout();


	// Layer에 Widget 추가
	UUserWidget* AddWidgetToLayer(EUILayer Layer, TSubclassOf<UUserWidget> WidgetClass);
	// Layer에서 Widget 제거
	void RemoveWidgetFromLayer(EUILayer Layer, UUserWidget* Widget);
};
