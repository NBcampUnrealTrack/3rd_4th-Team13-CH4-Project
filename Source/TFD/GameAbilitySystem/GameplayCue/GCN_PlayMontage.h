// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GCN_PlayMontage.generated.h"

UCLASS()
class TFD_API AGCN_PlayMontage : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage")
	UAnimMontage* MontageToPlay;

protected:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	
	// Cue 발동 시
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override
	{
		UE_LOG(LogTemp, Log, TEXT("GameplayCue Montage OnActive"))
		if (!MontageToPlay || !MyTarget)
			return false;

		// 캐릭터로 캐스팅
		APawn* Pawn = Cast<APawn>(MyTarget);
		if (!Pawn) return false;

		USkeletalMeshComponent* MeshComp = Pawn->FindComponentByClass<USkeletalMeshComponent>();
		if (!MeshComp) return false;

		MeshComp->GetAnimInstance()->Montage_Play(MontageToPlay);
		return true;
	}

	// Cue 제거 시
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override
	{
		if (!MontageToPlay || !MyTarget)
			return false;

		APawn* Pawn = Cast<APawn>(MyTarget);
		if (!Pawn) return false;

		USkeletalMeshComponent* MeshComp = Pawn->FindComponentByClass<USkeletalMeshComponent>();
		if (!MeshComp) return false;

		MeshComp->GetAnimInstance()->Montage_Stop(0.08f, MontageToPlay);
		return true;
	}
};