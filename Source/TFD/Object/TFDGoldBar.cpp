// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDGoldBar.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "TFDNativeGameplayTags.h"
#include "GameFramework/Character.h"
#include "Character/TFDCharacter.h"
#include "Controller/TFDPlayerController.h"
// Sets default values
ATFDGoldBar::ATFDGoldBar()
{
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

// Called when the game starts or when spawned
void ATFDGoldBar::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATFDGoldBar::OnOverlapBegin);

}

void ATFDGoldBar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GoldRewardEffect)
		return;

	if (ATFDCharacter* Player = Cast<ATFDCharacter>(OtherActor))
	{
		//ASC가져오는 이유 =  모든 능력치/효과 들고있음 , 어빌리티실행,이 펙트적용 
		if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		{
			if (ASC->HasMatchingGameplayTag(AllowedTeamTag))
			{
				ASC->ApplyGameplayEffectToSelf(GoldRewardEffect.GetDefaultObject(), 1.f, ASC->MakeEffectContext());
				Destroy();
			}
		}
	}
}

// Called every frame
void ATFDGoldBar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

