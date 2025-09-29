// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "EUIDefine.h"
#include "Components/CanvasPanel.h"
#include "GameUIRouterSubsystem.generated.h"

class UHUDLayoutWidget;
class UPlayingWidget;
class UResultWidget;
class UMiniMapWidget;
class UReleaseWidget;
class UUW_SkillSlot;
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
	::EHUDLayout PreviousLayout;
	// 현재 HUD Layout 추적
	::EHUDLayout CurrentLayout;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHUDLayoutWidget> HUDLayoutClass;

	UPROPERTY()
	UHUDLayoutWidget* HUDLayout;


public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateHUD();
	void SetHUDLayoutClass(TSubclassOf<UHUDLayoutWidget> InClass);

	// HUD 레이아웃 변경 (예: MainHUD → SpectatorHUD)
	UFUNCTION(BlueprintCallable)
	void PushLayout(EHUDLayout LayoutType);

	// HUD 레이아웃 제거 (뒤로가기)
	UFUNCTION(BlueprintCallable)
	void PopLayout();


	// Layer에 Widget 추가
	UFUNCTION(BlueprintCallable)
	UUserWidget* AddWidgetToLayer(EUILayer Layer, TSubclassOf<UUserWidget> WidgetClass);
	// Layer에서 Widget 제거
	UFUNCTION(BlueprintCallable)
	void RemoveWidgetFromLayer(EUILayer Layer, UUserWidget* Widget);

	UPROPERTY()
	UPlayingWidget* PlayingWidget = nullptr;

	UPROPERTY()
	UResultWidget* ResultWidget = nullptr;

	UPROPERTY()
	UMiniMapWidget* MiniMapWidget = nullptr;

	UPROPERTY()
	UReleaseWidget* ReleaseWidget = nullptr;

	UPROPERTY()
	UUW_SkillSlot* SkillSlotWidget = nullptr;
};
