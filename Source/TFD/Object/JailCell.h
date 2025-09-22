// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameAbilitySystem/Ability/TFDGameplayAbility.h"
#include "JailCell.generated.h"

class ATFDCharacterBase;
class ATFDGameState;
class UBoxComponent;
class UStaticMeshComponent;
class ATFDPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInOpenJailBox);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterOutOpenJailBox);

UCLASS()
class TFD_API AJailCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJailCell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 감옥 입장/퇴장 확인 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Collision")
	UBoxComponent* JailCellBox;

	//다른 도둑이 열기 시도할 수 있는 위치 파악 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Collision")
	UBoxComponent* OpenJailCellBox;


	UPROPERTY(VisibleAnywhere, Category = "Jail")
	TArray<TWeakObjectPtr<ATFDCharacterBase>> InCharacters;

	UPROPERTY(VisibleAnywhere, Category = "Jail")
	TArray<TWeakObjectPtr<ATFDPlayerState>> CharactersState;

	/** 감옥 입장 */
	void EnterJail(ATFDCharacterBase* Character);

	/** 감옥에서 나가기 */
	void Release(ATFDCharacterBase* Character);

	/** 감옥 벽 3초간 숨기기 */
	UFUNCTION(BlueprintCallable, Category = "Jail|Mesh")
	void HideWallsTemporarily();

	/** 실제 벽 다시 나타내기 */
	UFUNCTION()
	void ShowWalls();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterInOpenJailBox OnCharacterInOpenJailBox;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterOutOpenJailBox OnCharacterOutOpenJailBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnJailCellBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJailCellBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnOpenJailCellBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOpenJailCellBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(Replicated)
	ATFDGameState* CachedGameState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> CancleArrestedEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> ApplyReleaseAbilityEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> CancleReleaseAbilityEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Mesh")
	UStaticMeshComponent* WallFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Mesh")
	UStaticMeshComponent* WallBack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Mesh")
	UStaticMeshComponent* WallLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jail|Mesh")
	UStaticMeshComponent* WallRight;

	FTimerHandle ShowWallTimer;

};
