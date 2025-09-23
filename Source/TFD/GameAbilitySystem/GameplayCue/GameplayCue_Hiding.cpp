// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayCue/GameplayCue_Hiding.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

bool AGameplayCue_Hiding::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{

	if (!MyTarget)	return false;

	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	const int32 MatCount = Mesh->GetNumMaterials();
	for (int32 i = 0; i < MatCount; i++)
	{
		UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
		if (MID)
		{
			// 머티리얼에 "Opacity"라는 Scalar Parameter가 있어야 함
			MID->SetScalarParameterValue(FName("OpacityON/Off"), -1.0f); // -1 = 완전 투명
		}
	}

	return true;
}

bool AGameplayCue_Hiding::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{

	if (!MyTarget) return false;

	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	const int32 MatCount = Mesh->GetNumMaterials();
	for (int32 i = 0; i < MatCount; i++)
	{
		UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
		if (MID)
		{
			MID->SetScalarParameterValue(FName("OpacityOn/Off"), 1.0f); // 1 = 완전 불투명
		}
	}

	return true;
}


