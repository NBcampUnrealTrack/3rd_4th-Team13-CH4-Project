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
	FVector BoxOrigin = SpawningBox->GetComponentLocation(); 
 
	UWorld* World = GetWorld(); 
	if (!World) 
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null, returning safe position"));
		return BoxOrigin + FVector(0, 0, 100.f); 
	}
 
	const float SpawnHeightOffset = 50.f; // 스폰 높이 오프셋
	const int32 MaxAttempts = 10; // 최대 시도 횟수
    
	for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
	{
		// 박스 내 랜덤 XY 좌표 생성
		FVector RandomXY(
			BoxOrigin.X + FMath::FRandRange(-BoxExtent.X, BoxExtent.X), 
			BoxOrigin.Y + FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y), 
			BoxOrigin.Z
		); 
        
		// LineTrace로 바닥 찾기
		FHitResult Hit; 
		FVector TraceStart = FVector(RandomXY.X, RandomXY.Y, BoxOrigin.Z + BoxExtent.Z + 500.f); 
		FVector TraceEnd   = FVector(RandomXY.X, RandomXY.Y, BoxOrigin.Z - BoxExtent.Z - 500.f); 
     
		FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnTrace), true); 
		Params.AddIgnoredActor(GetOwner()); 
     
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params)) 
		{ 
			FVector GroundLocation = Hit.Location + FVector(0, 0, SpawnHeightOffset);
			UE_LOG(LogTemp, Log, TEXT("Attempt %d: LineTrace spawn at: %s"), Attempt + 1, *GroundLocation.ToString());
			return GroundLocation; 
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Attempt %d: LineTrace failed at XY(%.1f, %.1f)"), Attempt + 1, RandomXY.X, RandomXY.Y);
	}
 
	// 모든 시도 실패시 안전한 위치 (박스 상단 위)
	FVector SafeLocation = BoxOrigin + FVector(0, 0, BoxExtent.Z + SpawnHeightOffset);
	UE_LOG(LogTemp, Error, TEXT("All spawn attempts failed! Using safe position: %s"), *SafeLocation.ToString());
	return SafeLocation;
	
	 // FVector BoxExtent = SpawningBox->GetScaledBoxExtent(); 
  //   FVector BoxOrigin = SpawningBox->GetComponentLocation(); 
  //
  //   UWorld* World = GetWorld(); 
  //   if (!World) 
  //   {
  //       UE_LOG(LogTemp, Warning, TEXT("World is null, returning safe position"));
  //       return BoxOrigin + FVector(0, 0, 100.f); 
  //   }
  //
  //   const float SpawnHeightOffset = 50.f; // 오프셋을 50으로 증가
  //   const int32 MaxAttempts = 10; // 최대 시도 횟수
  //   
  //   for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
  //   {
  //       // 박스 내 랜덤 XY 좌표 생성
  //       FVector RandomXY(
  //           BoxOrigin.X + FMath::FRandRange(-BoxExtent.X, BoxExtent.X), 
  //           BoxOrigin.Y + FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y), 
  //           BoxOrigin.Z
  //       ); 
  //       
  //       // 1. 먼저 LineTrace로 확실한 바닥 찾기
  //       FHitResult Hit; 
  //       FVector TraceStart = FVector(RandomXY.X, RandomXY.Y, BoxOrigin.Z + BoxExtent.Z + 500.f); 
  //       FVector TraceEnd   = FVector(RandomXY.X, RandomXY.Y, BoxOrigin.Z - BoxExtent.Z - 500.f); 
  //    
  //       FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnTrace), true); 
  //       Params.AddIgnoredActor(GetOwner()); 
  //    
  //       if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params)) 
  //       { 
  //           FVector GroundLocation = Hit.Location + FVector(0, 0, SpawnHeightOffset);
  //           
  //           // 2. NavMesh 확인 (옵션)
  //           if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World)) 
  //           { 
  //               FNavLocation NavLocation; 
  //               if (NavSys->ProjectPointToNavigation(GroundLocation, NavLocation, FVector(100, 100, 100))) 
  //               { 
  //                   FVector FinalLocation = NavLocation.Location + FVector(0, 0, SpawnHeightOffset);
  //                   UE_LOG(LogTemp, Log, TEXT("Attempt %d: NavMesh+LineTrace spawn at: %s"), Attempt + 1, *FinalLocation.ToString());
  //                   return FinalLocation; 
  //               }
  //           }
  //           
  //           // NavMesh 없어도 바닥 위면 OK
  //           UE_LOG(LogTemp, Log, TEXT("Attempt %d: LineTrace spawn at: %s"), Attempt + 1, *GroundLocation.ToString());
  //           return GroundLocation; 
  //       }
  //       
  //       UE_LOG(LogTemp, Warning, TEXT("Attempt %d: LineTrace failed"), Attempt + 1);
  //   }
  //
  //   // 모든 시도 실패시 안전한 위치
  //   FVector SafeLocation = BoxOrigin + FVector(0, 0, BoxExtent.Z + SpawnHeightOffset);
  //   UE_LOG(LogTemp, Error, TEXT("All spawn attempts failed! Using safe position: %s"), *SafeLocation.ToString());
  //   return SafeLocation; 
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
