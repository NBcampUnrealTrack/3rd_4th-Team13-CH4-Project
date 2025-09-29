// TFDPickupSkillItem.cpp
#include "Object/SkillItem/TFDPickupSkillItem.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Object/SkillItem/Subsystem/TFDSkillEventSubsystem.h" // 서브시스템 헤더 추가
#include "Controller/TFDAIController.h"
#include "Engine/World.h" // GetWorld() 사용을 위해 추가

ATFDPickupSkillItem::ATFDPickupSkillItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// 투명화 스킬 태그 설정 (에디터에서 수정 가능)
	PickupSkillItemSkillTag = FGameplayTag::RequestGameplayTag(FName("Ability.Thief.Invisibility"));

	// 초기화
	bIsPickedUp = false;
	bIsDestroyed = false;
	bIsProcessingPickup = false;
}

void ATFDPickupSkillItem::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][BeginPlay] - Tag: %s"), *PickupSkillItemSkillTag.ToString());
	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][BeginPlay] Before adding delegate to OnComponentBeginOverlap"));

	// 부모 클래스에서 선언된 CollisionComp를 사용하여 오버랩 이벤트만 바인딩
	if (CollisionComp)
	{
		// 부모 클래스에서 설정된 오버랩 이벤트를 제거하고 자식에서만 처리하도록 설정
		CollisionComp->OnComponentBeginOverlap.RemoveAll(this);

		// 자식 클래스에서만 오버랩 이벤트를 추가
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATFDPickupSkillItem::OnOverlapSkill);
	}
	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][BeginPlay] Delegate added to OnComponentBeginOverlap"));
}

void ATFDPickupSkillItem::OnItemPickedUp()
{
	// 이미 아이템이 획득되었거나, 이미 파괴된 상태라면 아무것도 하지 않음
	if (bIsPickedUp || bIsDestroyed)
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] Item already picked up, destroyed, or being processed, skipping."));
		return;
	}

	// 아이템 획득 상태로 설정
	bIsPickedUp = true;

	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] - Broadcasting: %s"), *PickupSkillItemSkillTag.ToString());

	// 마지막으로 오버랩한 플레이어 정보가 필요 - OnOverlapSkill에서 저장
	if (LastOverlapPawn)
	{
		BroadcastSkillItemObtainedToPlayer(PickupSkillItemSkillTag, LastOverlapPawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] No target player found"));
	}

	// 아이템 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 아이템 파괴 상태로 설정
	bIsDestroyed = true;

	// 즉시 콜리전 비활성화 (추가 오버랩 방지)
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.RemoveAll(this);
	}

	// 딜레이를 두고 아이템 제거 (즉시 제거하면 로직이 완전히 끝나지 않을 수 있음)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ATFDPickupSkillItem::DelayedDestroy,
			0.1f, // 0.1초 후 제거
			false
		);
	}
	else
	{// World가 없으면 즉시 제거
		Destroy();
	}
	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] Item destroyed"));
}

void ATFDPickupSkillItem::DelayedDestroy()
{
	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][DelayedDestroy] Item destroyed"));
	Destroy();
}

void ATFDPickupSkillItem::OnOverlapSkill(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 서버에서만 처리
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapSkill] No authority, ignoring overlap on client. Actor: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
		return;
	}

	// 오버랩이 발생했지만 이미 아이템을 획득하거나, 이미 파괴된 상태라면 처리하지 않음
	if (bIsPickedUp || bIsDestroyed || bIsProcessingPickup)
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapSkill] Item already picked up, destroyed, or being processed, ignoring overlap. Flags: PickedUp=%d, Destroyed=%d, Processing=%d"),
			bIsPickedUp, bIsDestroyed, bIsProcessingPickup);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Overlap!!! Actor: %s, Setting bIsProcessingPickup=true, Authority: %d"),
		OtherActor ? *OtherActor->GetName() : TEXT("NULL"), HasAuthority());

	// 다른 액터가 아이템과 오버랩 시 아이템을 주운 것으로 간주
	if (OtherActor && OtherActor != this && OtherComp)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			// Controller 체크를 더 정확하게
			AController* Controller = Pawn->GetController();
			if (Controller && Controller->IsPlayerController())
			{
				// 즉시 처리 플래그 설정
				bIsProcessingPickup = true;

				// 콜리전을 즉시 비활성화
				if (CollisionComp)
				{
					CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					CollisionComp->OnComponentBeginOverlap.RemoveAll(this);
				}

				UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Valid player found: %s, Controller: %s"), *Pawn->GetName(), *Controller->GetName());

				// 오버랩한 플레이어 저장
				LastOverlapPawn = Pawn;

				// 아이템을 주운 것으로 간주
				OnItemPickedUp();
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] AI or non-player actor ignored: %s (Controller: %s)"),
					*OtherActor->GetName(), Controller ? *Controller->GetName() : TEXT("NULL"));
				// 유효하지 않은 액터면 플래그 되돌리기
				bIsProcessingPickup = false;
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Non-pawn actor ignored: %s"), *OtherActor->GetName());

			// 유효하지 않은 액터면 플래그 되돌리기
			bIsProcessingPickup = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Invalid overlap with: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

		// 유효하지 않은 오버랩이면 플래그 되돌리기
		bIsProcessingPickup = false;
	}
}

void ATFDPickupSkillItem::BroadcastSkillItemObtainedToPlayer(FGameplayTag SkillTag, APawn* TargetPawn)
{
	UTFDSkillEventSubsystem* SkillEventSubsystem = UTFDSkillEventSubsystem::GetSkillEventSubsystemSimple(this);
	if (SkillEventSubsystem)
	{
		//// 직접 스킬 추가 방식 사용
		//bool bSuccess = SkillEventSubsystem->AddSkillDirectlyToPlayer(SkillTag, TargetPawn, 1);

		//if (bSuccess)
		//{
		//	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][BroadcastSkillItemObtainedToPlayer] Successfully added skill %s to player %s"),
		//		*SkillTag.ToString(), *TargetPawn->GetName());
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("[ATFDPickupSkillItem][BroadcastSkillItemObtainedToPlayer] Failed to add skill to player"));

		//	// 직접 추가 실패 시 이벤트 브로드캐스트로 폴백
		//	SkillEventSubsystem->BroadcastSkillItemObtained(SkillTag, TargetPawn);
		//}

		SkillEventSubsystem->BroadcastSkillItemObtained(SkillTag, TargetPawn);
	}
}

//void ATFDPickupSkillItem::ServerRequestPickup_Implementation(APawn* RequestingPawn)
//{
//	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][ServerRequestPickup] Server received pickup request from %s"),
//		RequestingPawn ? *RequestingPawn->GetName() : TEXT("NULL"));
//
//	// 서버에서 다시 한번 유효성 검사
//	if (bIsPickedUp || bIsDestroyed || bIsProcessingPickup)
//	{
//		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][ServerRequestPickup] Item already processed on server, ignoring request"));
//		return;
//	}
//
//	if (RequestingPawn && RequestingPawn->GetController() && RequestingPawn->GetController()->IsPlayerController())
//	{
//		LastOverlapPawn = RequestingPawn;
//		OnItemPickedUp();
//	}
//}
//
//bool ATFDPickupSkillItem::ServerRequestPickup_Validate(APawn* RequestingPawn)
//{
//	// 기본적인 유효성 검사
//	return RequestingPawn != nullptr;
//}