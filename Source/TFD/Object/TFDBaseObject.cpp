// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDBaseObject.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "Character/TFDCharacterBase.h"
#include "Component/TFDProjectileMovementComponent.h"
#include "GameMode/TFDGameMode.h"


// Sets default values
ATFDBaseObject::ATFDBaseObject()
{
	DefaultCreater();
}

// Called when the game starts or when spawned
void ATFDBaseObject::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATFDBaseObject::OnOverlapBegin);
	SetAllowedTeamTag();
}

void ATFDBaseObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (!CollisionEffect)
		return;

	ATFDCharacterBase* Player = Cast<ATFDCharacterBase>(OtherActor);
	if (!Player)
		return;

	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (!ASC)
		return;

	if (!HasAllowedTeamTag(ASC))
		return;

	// 실제 로직 실행
	ApplyEffectAndDestroy(ASC);
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
}


void ATFDBaseObject::SetAllowedTeamTag()
{
	ATFDGameMode* pGameMode = GetWorld()->GetAuthGameMode<ATFDGameMode>();
	if (!pGameMode)
		return;

	AllowedTeamTag = pGameMode->GetDTAllowedTeamTagContainer(ItemTag);
}

bool ATFDBaseObject::HasAllowedTeamTag(UAbilitySystemComponent* ASC) const
{
	// 배열을 컨테이너로 변환
	FGameplayTagContainer AllowedTagContainer;
	for (const FGameplayTag& Tag : AllowedTeamTag)
	{
		AllowedTagContainer.AddTag(Tag);
	}

	// 플레이어가 허용된 팀 태그 중 하나라도 가지고 있는지 확인
	return ASC->HasAnyMatchingGameplayTags(AllowedTagContainer);
}

void ATFDBaseObject::ApplyEffectAndDestroy(UAbilitySystemComponent* ASC)
{
	// 게임플레이 이펙트 적용
	ASC->ApplyGameplayEffectToSelf(CollisionEffect.GetDefaultObject(), 1.f, ASC->MakeEffectContext());

	// GameState에 아이템 제거 알림
	NotifyItemRemoval();

	// 오브젝트 파괴
	Destroy();
}

void ATFDBaseObject::NotifyItemRemoval()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	ATFDGameState* GameState = World->GetGameState<ATFDGameState>();
	if (!GameState)
		return;

	GameState->RemoveAllowedItem(this);
}
