// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDSpawnpoint.h"

#include "AllowTeamTag.h"
#include "Components/BillboardComponent.h"
#include "GameMode/TFDGameMode.h"

// Sets default values
ATFDSpawnpoint::ATFDSpawnpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;
}

void ATFDSpawnpoint::SpawnItem()
{
	if (SpawnedItem.IsValid())
	{
		// 이미 아이템이 있으면 스폰하지 않음
		return;
	}
	
	if (ItemClass == nullptr)
		return;

	ATFDBaseObject* NewSpawnedItem = GetWorld()->SpawnActor<ATFDBaseObject>(
		ItemClass,
		GetActorLocation(),
		GetActorRotation()
	);

	if (!NewSpawnedItem)
		return;

	SpawnedItem = NewSpawnedItem;
	
	
	// 아이템 소멸 시 콜백 등록
	NewSpawnedItem->OnDestroyed.AddDynamic(this,&ATFDSpawnpoint::OnSpawnedItemDestroyed);

	
	if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
	{
		const FGameplayTagContainer& TeamTags = NewSpawnedItem->GetAllowedTeamTag();

		if (TeamTags.HasTag(TAG_Team_Cop))
		{
			GS->PoliceMapItemArray.Add(MakeWeakObjectPtr(NewSpawnedItem));
		}
		else if (TeamTags.HasTag(TAG_Team_Thief))
		{
			GS->ThiefMapItemArray.Add(MakeWeakObjectPtr(NewSpawnedItem));
		}
	}
}

// Called when the game starts or when spawned
void ATFDSpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	SetAllowedTeamTag();
}

void ATFDSpawnpoint::OnSpawnedItemDestroyed(AActor* DestroyedActor)
{
	SpawnedItem.Reset();
    
	// 일정 시간 후 다시 스폰
	GetWorld()->GetTimerManager().SetTimer(
		ItemPeriodSpawnTimerHandle,
		this,
		&ATFDSpawnpoint::SpawnItem,
		SpawnPeriodSec,
		false
	);
	
}

void ATFDSpawnpoint::SetAllowedTeamTag()
{
	ATFDGameMode* pGameMode = GetWorld()->GetAuthGameMode<ATFDGameMode>();
	if (!pGameMode)
		return;
	UE_LOG(LogTemp, Display, TEXT("TFDSpawnpoint setting"));

	AllowedTeamTag = pGameMode->GetDTAllowedTeamTagContainer(ItemTag);


	ItemClass = pGameMode->GetDTAllowedTeamTag_Item(ItemTag);

	SpawnPeriodSec = pGameMode->GetDTAllowedTeamTag_Period(ItemTag);
}

// Called every frame
void ATFDSpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ATFDSpawnpoint::GetSpawnPointLocation()
{
	return Billboard->GetComponentLocation();
}

FGameplayTagContainer ATFDSpawnpoint::GetAllowedTeamTag()
{
	return AllowedTeamTag;
}

bool ATFDSpawnpoint::CheckItemTag(FGameplayTag InTag)
{
	bool result = false;
	if (ItemTag == InTag)
		result = true;
	return result;
}
