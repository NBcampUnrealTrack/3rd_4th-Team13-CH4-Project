// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TFDSpawnpoint.generated.h"

UCLASS()
class TFD_API ATFDSpawnpoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATFDSpawnpoint();

public:
	void SpawnItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetSpawnPointLocation();
	FGameplayTagContainer GetAllowedTeamTag();

	bool CheckItemTag(FGameplayTag InTag);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category="Visual")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, Category="GAS")
	FGameplayTag ItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	FGameplayTagContainer AllowedTeamTag;

	// 스폰할 아이템
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TSubclassOf<AActor> ItemClass;

	// 스폰할 아이템 주기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tag")
	float SpawnPeriodSec;

	//만들어진 아이템
	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnedItem;

	UFUNCTION()
	void OnSpawnedItemDestroyed(AActor* DestroyedActor);

private:
	void SetAllowedTeamTag();
private:
	FTimerHandle ItemPeriodSpawnTimerHandle;
};
