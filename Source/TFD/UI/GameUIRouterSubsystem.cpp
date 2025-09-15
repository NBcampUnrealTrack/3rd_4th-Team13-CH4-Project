// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUIRouterSubsystem.h"
#include "Widget/UHUDLayoutWidget.h"


void UGameUIRouterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// LocalPlayer 전용이라 서버에서는 절대 실행되지 않음
	if (!HUDLayoutClass) return;

	HUDLayout = CreateWidget<UHUDLayoutWidget>(GetWorld(), HUDLayoutClass);
	if (HUDLayout)
	{
		HUDLayout->AddToViewport();
		CurrentLayout = EHUDLayout::InGame; // 기본 Layout
	}
}

void UGameUIRouterSubsystem::CreateHUD()
{
	// LocalPlayer 전용이라 서버에서는 절대 실행되지 않음
	if (!HUDLayoutClass) return;

	HUDLayout = CreateWidget<UHUDLayoutWidget>(GetWorld(), HUDLayoutClass);
	if (HUDLayout)
	{
		HUDLayout->AddToViewport();
		CurrentLayout = EHUDLayout::InGame; // 기본 Layout
	}
}

void UGameUIRouterSubsystem::SetHUDLayoutClass(TSubclassOf<UHUDLayoutWidget> InClass)
{
	HUDLayoutClass = InClass;
}

void UGameUIRouterSubsystem::PushLayout(EHUDLayout LayoutType)
{
	if (!HUDLayoutClass) return;

	// 이미 같은 Layout이면 아무 처리도 안 함
	if (CurrentLayout == LayoutType) return;

	// 기존 Layout 제거
	if (HUDLayout)
	{
		HUDLayout->RemoveFromParent();
		HUDLayout = nullptr;
	}

	// 새로운 Layout 생성
	HUDLayout = CreateWidget<UHUDLayoutWidget>(GetWorld(), HUDLayoutClass);
	if (HUDLayout)
	{
		HUDLayout->AddToViewport();
		CurrentLayout = LayoutType;

		// 필요하면 초기화 이벤트 호출
		// HUDLayout->OnLayoutInitialized(LayoutType); // BlueprintImplementableEvent 등
	}
}

void UGameUIRouterSubsystem::PopLayout()
{
	if (HUDLayout)
	{
		HUDLayout->RemoveFromParent();
		HUDLayout = nullptr;
	}

	CurrentLayout = PreviousLayout;

	if (HUDLayoutClass)
	{
		HUDLayout = CreateWidget<UHUDLayoutWidget>(GetWorld(), HUDLayoutClass);
		if (HUDLayout)
		{
			HUDLayout->AddToViewport();
		}
	}
}

UUserWidget* UGameUIRouterSubsystem::AddWidgetToLayer(EUILayer Layer, TSubclassOf<UUserWidget> WidgetClass)
{
	if (!HUDLayout || !WidgetClass) return nullptr;

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	HUDLayout->AddToLayer(Layer, Widget);
	return Widget;
}

void UGameUIRouterSubsystem::RemoveWidgetFromLayer(EUILayer Layer, UUserWidget* Widget)
{
	if (!HUDLayout || !Widget) return;
	HUDLayout->RemoveFromLayer(Layer, Widget);
}
