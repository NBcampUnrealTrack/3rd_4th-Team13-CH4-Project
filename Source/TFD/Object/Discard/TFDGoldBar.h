// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TFDGoldBar.generated.h"


class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;

UCLASS()
class TFD_API ATFDGoldBar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATFDGoldBar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// 적용할 GameplayEffect (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> GoldRewardEffect;

	UPROPERTY(EditAnywhere, Category="GAS")
	FGameplayTag		ItemTag;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="GAS")
	TArray<FGameplayTag>		AllowedTeamTag;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	void SetAllowedTeamTag();

};
