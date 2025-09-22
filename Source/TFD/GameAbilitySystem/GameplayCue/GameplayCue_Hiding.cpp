// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayCue/GameplayCue_Hiding.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

bool AGameplayCue_Hiding::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{

	if (!MyTarget)	return false;

	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	Mesh->SetVisibility(false, true);

	return true;
}

bool AGameplayCue_Hiding::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{

	if (!MyTarget) return false;

	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	Mesh->SetVisibility(true, true);

	return true;
}


