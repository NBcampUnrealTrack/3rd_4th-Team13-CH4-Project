// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameUIRouterSubsystem.generated.h"

/**
 * UI 관리 서브 시스템
 * - LocalPlayerSubsystem 기반으로 자동 클라 전용
 * - HUD Layout 관리, Layer별 Widget 추가/제거 제공
 */
UCLASS()
class TFD_API UGameUIRouterSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
private:
	TMap<EUILayer, UUserWidget*> ActiveWidgets;
	
	// 이전 Layout 저장 (PopLayout용) 한 번에 훨씬 많은 레이아웃이 활성화 되면 TArray로 확장필요.
	EHUDLayout PreviousLayout;
	// 현재 HUD Layout 추적
	EHUDLayout CurrentLayout;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHUDLayoutWidget> HUDLayoutClass;

	UPROPERTY()
	UHUDLayoutWidget* HUDLayout;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// HUD 레이아웃 변경 (예: MainHUD → SpectatorHUD)
	void PushLayout(EHUDLayout LayoutType);

	// HUD 레이아웃 제거 (뒤로가기)
	void PopLayout();


	// Layer에 Widget 추가
	UUserWidget* AddWidgetToLayer(EUILayer Layer, TSubclassOf<UUserWidget> WidgetClass);
	// Layer에서 Widget 제거
	void RemoveWidgetFromLayer(EUILayer Layer, UUserWidget* Widget);
};
