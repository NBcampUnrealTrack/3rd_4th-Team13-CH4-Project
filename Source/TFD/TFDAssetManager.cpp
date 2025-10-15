// Fill out your copyright notice in the Description page of Project Settings.


#include "TFDAssetManager.h"
#include "TFDDA_AbilityCatalog.h"
#include "AbilitySystemGlobals.h"

void UTFDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// AbilitySystemGlobals 초기화
	UAbilitySystemGlobals::Get().InitGlobalData();

	// Primary Asset ID 생성
	const FPrimaryAssetId CatalogAssetId(TEXT("DataAsset"), TEXT("TFDDA_AbilityCatalog"));

	// 비동기 로딩 델리게이트
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(
		this,
		&UTFDAssetManager::OnAbilityCatalogLoaded
	);

	LoadPrimaryAsset(CatalogAssetId, TArray<FName>(), Delegate);
	
}

UTFDAssetManager& UTFDAssetManager::Get()
{
	UAssetManager& Manager = UAssetManager::Get();
	return static_cast<UTFDAssetManager&>(Manager);
}

void UTFDAssetManager::OnAbilityCatalogLoaded()
{
	const FPrimaryAssetId CatalogAssetId(TEXT("DataAsset"), TEXT("TFDDA_AbilityCatalog"));

	TFDDA_AbilityCatalog = Cast<UTFDDA_AbilityCatalog>(
		GetPrimaryAssetObject(CatalogAssetId)
	);

	if (!TFDDA_AbilityCatalog)
	{
		UE_LOG(LogTemp, Error, TEXT("Ability Catalog failed to load!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Ability Catalog loaded successfully!"));
}
