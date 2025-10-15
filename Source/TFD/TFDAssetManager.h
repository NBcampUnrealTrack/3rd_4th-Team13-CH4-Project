// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFDDA_AbilityCatalog.h"
#include "Engine/AssetManager.h"
#include "TFDAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API UTFDAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;

	static UTFDAssetManager& Get();

private:

	void OnAbilityCatalogLoaded();

	UPROPERTY()
	UTFDDA_AbilityCatalog* TFDDA_AbilityCatalog;  
	
};
