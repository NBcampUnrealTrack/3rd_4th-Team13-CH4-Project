// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDBaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Component/TFDProjectileMovementComponent.h"
#include "TFDNativeGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ATFDBaseProjectile::ATFDBaseProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UTFDProjectileMovementComponent>(
		TEXT("TFDProjectileMovementComponent"));

	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 0.f;

	PrimaryActorTick.bCanEverTick = true;
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

void ATFDBaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasHit)
		return;

	FVector Start = GetActorLocation();
	FVector Velocity = ProjectileMovementComponent->Velocity;
	float SweepDistance = Velocity.Size() * DeltaTime;
	FVector End = Start + Velocity.GetSafeNormal() * SweepDistance;

	// 디버깅용 시각화
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), End, 30.f, 12, FColor::Green, false, 1.0f);

	// 충돌 체크
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetOwner())
		Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn, // 투사체 충돌용 채널
		FCollisionShape::MakeSphere(30.f),
		Params
	);

	if (bHit)
	{
		bHasHit = true;

		SpawnImpactVFX(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());


		// 서버 권한 로직
		if (HasAuthority())
		{
			if (BaseObjectParam.ImpactSound)
				UGameplayStatics::PlaySoundAtLocation(this, BaseObjectParam.ImpactSound, Hit.ImpactPoint);
		}
	}
	else
	{
		SetActorLocation(End, true);
	}
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

	ENetMode NetMode = GetNetMode();
	if (NetMode != NM_DedicatedServer)
	{
		SpawnMuzzleVFX(StartTransform.GetLocation(), StartTransform.GetRotation().Rotator());
	}

	// Muzzle Sound는 클라이언트/서버 동시에 재생해도 무방
	if (BaseObjectParam.MuzzleSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BaseObjectParam.MuzzleSound, StartTransform.GetLocation());
	}
}

void ATFDBaseProjectile::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                       bool bFromSweep, const FHitResult& SweepResult)
{
	// if (!OtherActor || OtherActor == this)
	// 	return;
	//
	// FVector SpawnLocation = SweepResult.ImpactPoint;
	// FRotator SpawnRotation = FRotator::ZeroRotator;

	// ENetMode NetMode = GetNetMode();
	// if (GetNetMode() != NM_DedicatedServer) 
	// {
	// 	SpawnImpactVFX(SpawnLocation, SpawnRotation);
	// }


	// if (HasAuthority())
	// {
	// 	if (BaseObjectParam.ImpactSound)
	// 	{
	// 		UGameplayStatics::PlaySoundAtLocation(this, BaseObjectParam.ImpactSound, SpawnLocation);
	// 	}
	// }

	Super::OnOverlapBegin_Implementation(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                     SweepResult);
}

void ATFDBaseProjectile::SpawnMuzzleVFX(const FVector& Location, const FRotator& Rotation)
{
	if (!BaseObjectParam.MuzzleVFX)
		return;

	UNiagaraComponent* MuzzleVFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		BaseObjectParam.MuzzleVFX,
		Location,
		Rotation,
		BaseObjectParam.MuzzleScale,
		true
	);

	if (MuzzleVFXComp)
	{
		MuzzleVFXComp->SetAutoDestroy(true);
		MuzzleVFXComp->SetFloatParameter(TEXT("User.PlayRate"), BaseObjectParam.ImpactVFXSpeed);

		UWorld* World = GetWorld();

		float EffectDuration = 1.0f; // 이펙트의 최대 지속 시간 (0.2~0.5초 정도가 일반적)
		World->GetTimerManager().SetTimer(MuzzleVFXTimerHandle, [MuzzleVFXComp]()
		                                  {
			                                  if (MuzzleVFXComp) { MuzzleVFXComp->DestroyComponent(); }
		                                  }, EffectDuration, false
		);
	}
}

void ATFDBaseProjectile::SpawnImpactVFX(const FVector& Location, const FRotator& Rotation)
{
	if (!BaseObjectParam.ImpactVFX)
		return;

	UNiagaraComponent* ImpactVFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		BaseObjectParam.ImpactVFX,
		Location,
		Rotation,
		BaseObjectParam.ImpactScale,
		true
	);

	if (ImpactVFXComp)
	{
		ImpactVFXComp->SetAutoDestroy(true);
		ImpactVFXComp->SetFloatParameter(TEXT("User.PlayRate"), BaseObjectParam.ImpactVFXSpeed);

		UWorld* World = GetWorld();

		float EffectDuration = 1.5f; // 이펙트의 최대 지속 시간 (0.2~0.5초 정도가 일반적)
		World->GetTimerManager().SetTimer(ImpactVFXTimerHandle, [ImpactVFXComp]()
		                                  {
			                                  if (ImpactVFXComp) { ImpactVFXComp->DestroyComponent(); }
		                                  }, EffectDuration, false
		);
	}
}
