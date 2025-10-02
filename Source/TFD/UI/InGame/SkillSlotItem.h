// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"
#include "SkillSlotItem.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;

UCLASS()
class TFD_API USkillSlotItem : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void UpdateSlot(const FTFDSkillSlot& InSlot, int32 SlotIndex);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> CooldownBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> UsageCountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> BindKey;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownText;

private:
	FTFDSkillSlot CurrentSlot;

	FTimerHandle CooldownTimerHandle;
	void UpdateCooldown();
};
