// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "TFDBaseObject.generated.h"

class UTFDProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;


UENUM(BlueprintType)
enum class EImpactRotationMethod : uint8
{
	// 충돌 지점의 표면 각도에 맞춰 회전합니다.
	AlignToImpactNormal,
	// 투사체가 날아가던 방향을 유지합니다.
	AlignToProjectile,
	// 항상 기본 회전값(0,0,0)을 사용합니다.
	ZeroRotation
};


UENUM(BlueprintType)
enum class EProjectileMovementType : uint8
{
	// 직선 이동: 일정한 방향과 속도로 곧게 이동
	Straight UMETA(DisplayName = "Straight"),
	// 포물선 이동: 중력 영향을 받아 곡선을 그리며 이동
	Arc UMETA(DisplayName = "Arc")
};


USTRUCT(BlueprintType)
struct FTFDBaseObjectParam
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Move",
		meta = (DisplayName = "Moveflag", EditConditionHides))
	bool bMoveFlag = false;

	// 투사체의 이동 방식을 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Movement", meta = (DisplayName = "이동 방식"))
	EProjectileMovementType MovementType = EProjectileMovementType::Straight;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Move",
		meta = (DisplayName = "가속도", EditConditionHides))
	float ProjectileSpeed = 1500.f;

	// true이면 비행 중 주변의 적을 탐지하고 유도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Homing", meta = (DisplayName = "유도 기능 활성화"))
	bool bEnableHoming = false;

	// 발사 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Muzzle",
		meta = (DisplayName = "발사 사운드"))
	TObjectPtr<USoundBase> MuzzleSound;

	// 발사 시 스폰할 나이아가라 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Muzzle",
		meta = (DisplayName = "발사 이펙트(VFX)"))
	TObjectPtr<UNiagaraSystem> MuzzleVFX;

	// 발사 이펙트의 크기를 조절
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Muzzle",
		meta = (DisplayName = "발사 이펙트 스케일", EditCondition = "MuzzleVFX != nullptr"))
	FVector MuzzleScale = FVector::OneVector;

	// 발사 이펙트의 재생 속도를 조절 (1.0 = 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Muzzle",
		meta = (DisplayName = "발사 이펙트 속도", EditCondition = "MuzzleVFX != nullptr", ClampMin = "0.01", UIMin = "0.01"))
	float MuzzleVFXSpeed = 1.0f;

	// 충돌 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 사운드"))
	TObjectPtr<USoundBase> ImpactSound;

	// 충돌 시 스폰할 나이아가라 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 이펙트(VFX)"))
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	// 충돌 이펙트의 크기를 조절
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 이펙트 스케일", EditCondition = "ImpactVFX != nullptr"))
	FVector ImpactScale = FVector::OneVector;

	// 충돌 이펙트의 재생 속도를 조절 (1.0 = 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 이펙트 속도", EditCondition = "ImpactVFX != nullptr", ClampMin = "0.01", UIMin = "0.01"))
	float ImpactVFXSpeed = 1.0f;

	// 충돌 이펙트의 초기 회전 방식을 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 이펙트 회전 방식", EditCondition = "ImpactVFX != nullptr"))
	EImpactRotationMethod RotationMethod = EImpactRotationMethod::AlignToImpactNormal;

	// 충돌 지점으로부터 이펙트가 얼마나 떨어져서 스폰될지 결정 (cm 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|Projectile|Effects|Impact",
		meta = (DisplayName = "충돌 이펙트 오프셋", EditCondition = "ImpactVFX != nullptr"))
	float ImpactOffset = 0.0f;
};

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
	void SetMovementComponent();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// 적용할 GameplayEffect (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> CollisionEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TFD|Components")
	TObjectPtr<UTFDProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category="GAS")
	FGameplayTag ItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	FGameplayTagContainer AllowedTeamTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties))
	FTFDBaseObjectParam BaseObjectParam;


	FGameplayEffectContextHandle CachedContext;
	TSubclassOf<UGameplayEffect> DamageEffectClass;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const FGameplayTagContainer& GetAllowedTeamTag() const { return AllowedTeamTag; }

	void SetBaseObjectParam(const FTFDBaseObjectParam& InBaseObjectParams);

private:
	void SetAllowedTeamTag();

private:
	FGameplayTag OwnerTeamTag;
	FTransform StartTransform = FTransform();
};
