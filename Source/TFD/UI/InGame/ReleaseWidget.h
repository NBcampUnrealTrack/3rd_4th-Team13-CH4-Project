// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ReleaseWidget.generated.h"


class UReleaseAbility;
class UGamePlayAbility;
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

	/** 진행 바 */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ReleaseProgressBar;

protected:

	/** 현재 차오르는 값 */
	float ElapsedTime = 0.f;

	/** 목표까지 도달하는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Release")
	float FillDuration = 3.f;

	/** 타이머 핸들 */
	FTimerHandle FillTimerHandle;

	void NativeConstruct() override;

	void UpdateReleaseProgress();

	void HandleReleaseAbilityActivated(const FGameplayEventData* Payload);

	void HandleReleaseAbilityCanceled(const FGameplayEventData* Payload);
};
