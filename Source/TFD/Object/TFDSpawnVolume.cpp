// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TFDSpawnVolume.h"

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

FVector ATFDSpawnVolume::GetRandomPointInVolume() const
{
	// FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// FVector BoxOrigin = SpawningBox->GetComponentLocation();
 //    
	// // 박스 범위 내에서 랜덤 포인트 생성
	// FVector RandomPoint = BoxOrigin + FVector(
	// 	FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
	// 	FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
	// 	FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	// );
 //    
	// // 라인트레이스를 위한 설정
	// FHitResult HitResult;
	// FVector TraceStart = FVector(RandomPoint.X, RandomPoint.Y, BoxOrigin.Z + BoxExtent.Z); // 박스 상단에서 시작
	// FVector TraceEnd = FVector(RandomPoint.X, RandomPoint.Y, BoxOrigin.Z - BoxExtent.Z);   // 박스 하단까지
 //    
	// // 라인트레이스 파라미터 설정
	// FCollisionQueryParams CollisionParams;
	// CollisionParams.bTraceComplex = true;
	// CollisionParams.AddIgnoredActor(GetOwner()); // 자신은 무시
 //    
	// // 월드에서 라인트레이스 실행
	// UWorld* World = GetWorld();
	// if (World && World->LineTraceSingleByChannel(
	// 	HitResult,
	// 	TraceStart,
	// 	TraceEnd,
	// 	ECollisionChannel::ECC_WorldStatic, // 또는 ECC_Visibility 사용 가능
	// 	CollisionParams))
	// {
	// 	// 바닥에 히트했을 경우 해당 위치 반환
	// 	return HitResult.Location;
	// }
 //    
	// // 바닥을 찾지 못했을 경우 박스 하단 중앙 반환 (fallback)
	// return FVector(RandomPoint.X, RandomPoint.Y, BoxOrigin.Z - BoxExtent.Z);

	//
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// 2) 박스 중심 위치
	FVector BoxOrigin = SpawningBox->GetComponentLocation();
	
	// 3) 각 축별로 -Extent ~ +Extent 범위의 무작위 값 생성
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
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

