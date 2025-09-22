// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ReleaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UReleaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 안내 텍스트 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReleaseText;
};
