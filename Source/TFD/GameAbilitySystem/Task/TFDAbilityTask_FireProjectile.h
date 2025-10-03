// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TFD/Object/TFDBaseProjectile.h"
#include "TFDAbilityTask_FireProjectile.generated.h"

class ATFDBaseProjectile;
class ATFDCharacterBase;

USTRUCT(BlueprintType)
struct FTFDFireProjectileParams
{
	GENERATED_BODY()

	// 발사할 투사체의 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Core", meta = (DisplayName = "투사체 클래스"))
	TSubclassOf<ATFDBaseProjectile> ProjectileClass;


	// --- 스폰 설정 ---

	// 발사체가 생성될 캐릭터 메시의 소켓 이름. 'None'일 경우 캐릭터의 발밑에서 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning", meta = (DisplayName = "발사 소켓 이름"))
	FName SocketName;

	// 소켓 위치에서 추가적으로 적용될 로컬 트랜스폼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning", meta = (DisplayName = "로컬 트랜스폼 오프셋"))
	FTransform ProjectileLocalTransform = FTransform();

	// 투사체가 파괴되기까지의 시간(초). 사거리 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning",
		meta = (DisplayName = "투사체 수명 (초)", ClampMin = "0.01", UIMin = "0.01"))
	float ProjectileLifeSpan = 1.5f;

	// 투사체 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task")
	FTFDBaseObjectParam BaseObjectParam;
};
//어빌리티에게 알리는 용도
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileFired);
/**
 * 
 */
UCLASS()
class TFD_API UTFDAbilityTask_FireProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TFD|Ability|Tasks",
		meta = (DisplayName = "Fire Projectile", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))

	static UTFDAbilityTask_FireProjectile* FireProjectile(
		UGameplayAbility* OwningAbility,
		const FTFDFireProjectileParams& Params
	);

	UPROPERTY(BlueprintAssignable)
	FOnProjectileFired OnProjectileFired;
	//
	// // 모든 발사가 끝났을 때 실행
	// UPROPERTY(BlueprintAssignable)
	// FFireProjectileFinishedDelegate OnFinished;
protected:
	virtual void Activate() override;
	virtual void EndTask() ;
private:
	//투사체 운동 처리
	void ShootProjectile();
	bool CheckProjectile(AActor& pActor, UAbilitySystemComponent& SourceASC);
	bool CalcSpawnTransform(ATFDCharacterBase* Character, FTransform& OutSpawnTransform);

	// 클라이언트 전용 시각 효과 투사체 생성
	void SpawnCosmeticProjectile();

private:
	//발사체 최종 트랜스폼
	FTransform SpawnWorldTransform;

	FTFDFireProjectileParams TaskParams;
};
