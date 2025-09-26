// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TFDSpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class TFD_API ATFDSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATFDSpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
	USceneComponent* Scene;
	// 스폰 영역을 담당할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, Category="Spawning")
	int32 SpawnNum = 10;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag ObjectTag;
	
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag AllowedTeamTag;
public:
	UFUNCTION()
	bool CheckTeamTag(FGameplayTag InTag);
	
	// 스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
	UFUNCTION(BlueprintCallable, Category="Spawning")
	FVector GetRandomPointInVolume() const;

	// 스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
	UFUNCTION(BlueprintCallable, Category="Spawning")
	FVector GetRandomPointInVolumeLineTrace() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
