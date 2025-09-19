// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TFD/Object/TFDBaseObject.h"
#include "AbilityTask_FireProjectile.generated.h"

class ATFDBaseObject;

USTRUCT(BlueprintType)
struct FTFDFireProjectileParams
{
	GENERATED_BODY()
	
	// 발사할 투사체의 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Core", meta = (DisplayName = "투사체 클래스"))
	TSubclassOf<ATFDBaseObject> ProjectileClass;


	// 발사체가 적에게 적용할 데미지 GameplayEffect 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Damage", meta = (DisplayName = "데미지 이펙트 클래스"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;


	// --- 스폰 설정 ---

	// 발사체가 생성될 캐릭터 메시의 소켓 이름. 'None'일 경우 캐릭터의 발밑에서 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning", meta = (DisplayName = "발사 소켓 이름"))
	FName		 SocketName;

	// 소켓 위치에서 추가적으로 적용될 로컬 트랜스폼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning",meta = (DisplayName = "로컬 트랜스폼 오프셋"))
	FTransform ProjectileLocalTransform = FTransform();

	// 투사체가 파괴되기까지의 시간(초). 사거리 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task|Spawning",meta = (DisplayName = "투사체 수명 (초)", ClampMin = "0.01", UIMin = "0.01"))
	float ProjectileLifeSpan = 3.0f;
	
	// 투사체 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Task")
	FTFDBaseObjectParam BaseObjectParam;

	
};

/**
 * 
 */
UCLASS()
class TFD_API UTFDAbilityTask_FireProjectile : public UAbilityTask
{
	GENERATED_BODY()
};
