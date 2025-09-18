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
	if (ItemClass == nullptr)
		return;

	GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetActorLocation(),
		GetActorRotation()
	);
}

// Called when the game starts or when spawned
void ATFDSpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	SetAllowedTeamTag();
}

void ATFDSpawnpoint::SetAllowedTeamTag()
{
	ATFDGameMode* pGameMode = GetWorld()->GetAuthGameMode<ATFDGameMode>();
	if (!pGameMode)
		return;
	UE_LOG(LogTemp, Display, TEXT("TFDSpawnpoint setting"));

	AllowedTeamTag = pGameMode->GetDTAllowedTeamTagContainer(ItemTag);


	ItemClass = pGameMode->GetDTAllowedTeamTag_Item(ItemTag);
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
