// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDSpawnVolume.h"

#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Sets default values
ATFDSpawnVolume::ATFDSpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트를 생성하고, 이 액터의 루트로 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
}

bool ATFDSpawnVolume::CheckTeamTag(FGameplayTag InTag)
{
	bool result = false;
	if (InTag == AllowedTeamTag)
		result = true;
	return result;
}

FVector ATFDSpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// 2) 박스 중심 위치
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	// 3) 각 축별로 -Extent ~ +Extent 범위의 무작위 값 생성
	// return BoxOrigin + FVector(
	// 	FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
	// 	FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
	// 	0.f
	// );
	FVector RandomLocation = BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		0.0f // Z는 빼버리고
	);

	FNavLocation ProjectedLocation;
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		if (NavSys->ProjectPointToNavigation(RandomLocation, ProjectedLocation, FVector(500, 500, 500)))
		{
			return ProjectedLocation.Location;
		}
	}

	return RandomLocation; // NavMesh 투영 실패 시 그냥 리턴
}


FVector ATFDSpawnVolume::GetRandomPointInVolumeLineTrace() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent(); 

	// 랜덤 X,Y
	FVector RandomXY(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		0.f
	);

	FVector BaseLocation = SpawningBox->GetComponentLocation() + RandomXY;

	UWorld* World = GetWorld();
	if (!World)
		return BaseLocation;

	// Z 기준 안전 범위
	float TraceStartZ = 10000.f;
	float TraceEndZ = -10000.f;
	float SpawnHeightOffset = 50.f;

	FHitResult Hit;
	FVector TraceStart = FVector(BaseLocation.X, BaseLocation.Y, TraceStartZ);
	FVector TraceEnd   = FVector(BaseLocation.X, BaseLocation.Y, TraceEndZ);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnTrace), true);
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(GetOwner());

	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		FVector GroundLocation = Hit.Location + FVector(0,0,SpawnHeightOffset);
		return GroundLocation;
	}

	// 실패하면 맵 최소 Z
	return FVector(BaseLocation.X, BaseLocation.Y, 300.f);

}

// Called when the game starts or when spawned
void ATFDSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATFDSpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
