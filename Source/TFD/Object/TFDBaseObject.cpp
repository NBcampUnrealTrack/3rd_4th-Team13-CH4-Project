// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDBaseObject.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "TFDNativeGameplayTags.h"
#include "Character/TFDCharacterBase.h"
#include "Component/TFDProjectileMovementComponent.h"
#include "GameMode/TFDGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATFDBaseObject::ATFDBaseObject()
{
	DefaultCreater();
}

// Called when the game starts or when spawned
void ATFDBaseObject::BeginPlay()
{
	Super::BeginPlay();
	SetMovementComponent();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATFDBaseObject::OnOverlapBegin);
	SetAllowedTeamTag();


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

void ATFDBaseObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (!CollisionEffect)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("GRE:1"));
	if (ATFDCharacterBase* Player = Cast<ATFDCharacterBase>(OtherActor))
	{
		// ASC 가져오기
		if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		{
			// 배열 → 컨테이너 변환
			FGameplayTagContainer AllowedTagContainer;
			for (const FGameplayTag& Tag : AllowedTeamTag)
			{
				AllowedTagContainer.AddTag(Tag);
			}

			// 플레이어가 허용된 팀 태그 중 하나라도 가지고 있다면
			if (ASC->HasAnyMatchingGameplayTags(AllowedTagContainer))
			{
				ASC->ApplyGameplayEffectToSelf(CollisionEffect.GetDefaultObject(), 1.f, ASC->MakeEffectContext());

				// GameState에 아이템 제거 알림
				if (UWorld* World = GetWorld())
				{
					if (ATFDGameState* GS = World->GetGameState<ATFDGameState>())
					{
						GS->RemoveAllowedItem(this);
					}
				}

				Destroy();
			}
		}
	}
}

// Called every frame
void ATFDBaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFDBaseObject::DefaultCreater()
{
	bReplicates = true;
	SetReplicateMovement(true);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
	CollisionComp->InitSphereRadius(50.f);
	//충돌감지설정 QueryOnly : 충돌감지만하고 물리적충돌X
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//모든충돌채널에 대한 기본반응설정 : ECR_Ignore(모든오브젝트와 충돌무시)
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	//특정 채널에 대한 예외설정 ECC_Pawn(Pawn채널만) , ECR_Overlap(pawn과는 겹침감지)
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);
	//메시는 충돌안함
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UTFDProjectileMovementComponent>(
		TEXT("TFDProjectileMovementComponent"));

	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 0.f;
}

void ATFDBaseObject::SetMovementComponent()
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

void ATFDBaseObject::SetBaseObjectParam(const FTFDBaseObjectParam& InBaseObjectParams)
{
	BaseObjectParam = InBaseObjectParams;
}

void ATFDBaseObject::SetAllowedTeamTag()
{
	ATFDGameMode* pGameMode = GetWorld()->GetAuthGameMode<ATFDGameMode>();
	if (!pGameMode)
		return;

	AllowedTeamTag = pGameMode->GetDTAllowedTeamTagContainer(ItemTag);
}
