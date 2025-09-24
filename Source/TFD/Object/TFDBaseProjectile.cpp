// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDBaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Component/TFDProjectileMovementComponent.h"
#include "TFDNativeGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ATFDBaseProjectile::ATFDBaseProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UTFDProjectileMovementComponent>(
	TEXT("TFDProjectileMovementComponent"));

	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 0.f;
}

void ATFDBaseProjectile::SetBaseObjectParam(const FTFDBaseObjectParam& InBaseObjectParams)
{
	BaseObjectParam = InBaseObjectParams;
}

void ATFDBaseProjectile::SetMovementComponent()
{
	//발사체 움직임 컴포넌트
	if (BaseObjectParam.bMoveFlag)
	{
		UE_LOG(LogTemp, Warning, TEXT("GRE:1"));
		ProjectileMovementComponent->bAutoActivate = true;
		ProjectileMovementComponent->InitialSpeed = BaseObjectParam.ProjectileSpeed;
		ProjectileMovementComponent->MaxSpeed = BaseObjectParam.ProjectileSpeed;
		//중력 영향X
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
		// 투사체가 속도방향을 따라 회전
		ProjectileMovementComponent->bRotationFollowsVelocity = true;

		// 발사체 방향 = 액터의 전방 방향
		FVector ShootDirection = GetActorForwardVector();
		ProjectileMovementComponent->Velocity = ShootDirection * BaseObjectParam.ProjectileSpeed;

		// 활성화
		ProjectileMovementComponent->Activate();
	}

}

void ATFDBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	BeginPlaySetting();
}

void ATFDBaseProjectile::BeginPlaySetting()
{
	SetMovementComponent();

	if (GetOwner())
	{
		UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (OwnerAsc)
		{
			// 주인의 태그 중 Team 카테고리에 속하는 태그를 찾아서 저장.
			OwnerTeamTag = OwnerAsc->GetOwnedGameplayTags().Filter(FGameplayTagContainer(TAG_Team)).
									 First();
		}
	}

	StartTransform = GetActorTransform();
	switch (BaseObjectParam.MovementType)
	{
	case EProjectileMovementType::Arc:

		break;

	case EProjectileMovementType::Straight:
	default:
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
		break;
	}


	if (BaseObjectParam.MuzzleVFX)
	{
		UNiagaraComponent* MuzzleVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			BaseObjectParam.MuzzleVFX,
			StartTransform.GetLocation(),
			StartTransform.GetRotation().Rotator(),
			BaseObjectParam.MuzzleScale,
			true
		);
	}

	if (BaseObjectParam.MuzzleSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BaseObjectParam.MuzzleSound, StartTransform.GetLocation());
	}
}
