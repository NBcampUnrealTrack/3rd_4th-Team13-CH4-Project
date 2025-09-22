// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/TFDBaseObject.h"
#include "TFDSlowItem.generated.h"

/**
 * 
 */
UCLASS()
class TFD_API ATFDSlowItem : public ATFDBaseObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATFDSlowItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	
	virtual void Tick(float DeltaTime) override;
};
