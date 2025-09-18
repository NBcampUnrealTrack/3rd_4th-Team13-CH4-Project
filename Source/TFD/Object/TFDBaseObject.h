// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TFDBaseObject.generated.h"


class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;

UCLASS()
class TFD_API ATFDBaseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATFDBaseObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
	
	void DefaultCreater();
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// 적용할 GameplayEffect (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> CollisionEffect;

	UPROPERTY(EditAnywhere, Category="GAS")
	FGameplayTag		ItemTag;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="GAS")
	FGameplayTagContainer		AllowedTeamTag;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SetAllowedTeamTag();
};
