// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GameplayCue/GameplayCue_Hiding.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

bool AGameplayCue_Hiding::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 0"));
	if (!MyTarget)	return false;
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 1"));
	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 2"));
	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	Mesh->SetVisibility(false, true);
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 3"));
	return true;
}

bool AGameplayCue_Hiding::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 7"));
	if (!MyTarget) return false;
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 4"));
	ACharacter* Char = Cast<ACharacter>(MyTarget);
	if (!Char) return false;
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 5"));
	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return false;

	Mesh->SetVisibility(true, true);
	UE_LOG(LogTemp, Warning, TEXT("AGameplayCue_Hiding: 6"));
	return true;
}


