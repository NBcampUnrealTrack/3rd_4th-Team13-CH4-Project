// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayCue/GameplayCue_Demerit.h"

#include "AnimInstance/TFDAnimInstanceBase.h"
#include "Character/TFDCharacterBase.h"
#include "Character/TFDAICharacter.h"

bool AGameplayCue_Demerit::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("GameplayCue_Demerit::OnActive called"));

	ACharacter* Character = Cast<ACharacter>(MyTarget);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnActive: MyTarget is not a character"));
		return false;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnActive: Mesh is null"));
		return false;
	}

	UTFDAnimInstanceBase* AnimInstance = Cast<UTFDAnimInstanceBase>(Mesh->GetAnimInstance());
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnActive: AnimInstance cast failed"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnActive: Playing demerit animation"));
	AnimInstance->PlayDemeritAnim();

	return true;
}

bool AGameplayCue_Demerit::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("GameplayCue_Demerit::OnRemove called"));

	ACharacter* Character = Cast<ACharacter>(MyTarget);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRemove: MyTarget is not a character"));
		return false;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRemove: Mesh is null"));
		return false;
	}

	UTFDAnimInstanceBase* AnimInstance = Cast<UTFDAnimInstanceBase>(Mesh->GetAnimInstance());
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRemove: AnimInstance cast failed"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnRemove: Ending demerit animation"));
	AnimInstance->DemeritAnimEnd();

	return true;
}

