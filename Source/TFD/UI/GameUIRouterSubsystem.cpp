// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUIRouterSubsystem.h"



void UGameUIRouterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// LocalPlayer 전용이라 서버에서는 절대 실행되지 않음
	InitializeHUD();
}