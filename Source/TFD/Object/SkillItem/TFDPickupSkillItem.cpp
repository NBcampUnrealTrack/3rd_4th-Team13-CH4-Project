// TFDPickupSkillItem.cpp
#include "Object/SkillItem/TFDPickupSkillItem.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Object/SkillItem/Subsystem/TFDSkillEventSubsystem.h" // 서브시스템 헤더 추가
#include "Controller/TFDAIController.h"

ATFDPickupSkillItem::ATFDPickupSkillItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// 투명화 스킬 태그 설정 (에디터에서 수정 가능)
	PickupSkillItemSkillTag = FGameplayTag::RequestGameplayTag(FName("Ability.Thief.Invisibility"));
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
		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] Item already picked up or destroyed, skipping."));
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

	// 콜리전 이벤트 제거 (아이템 파괴 시 오버랩 이벤트가 더 이상 발생하지 않도록)
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.RemoveAll(this);
	}

	// 아이템 제거
	Destroy();
	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnItemPickedUp] Item destroyed"));
}

void ATFDPickupSkillItem::OnOverlapSkill(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 오버랩이 발생했지만 이미 아이템을 획득하거나, 이미 파괴된 상태라면 처리하지 않음
	if (bIsPickedUp || bIsDestroyed)
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapSkill] Item already picked up or destroyed, ignoring overlap."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Overlap!!!"));

	// 다른 액터가 아이템과 오버랩 시 아이템을 주운 것으로 간주
	if (OtherActor && OtherActor != this && OtherComp)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn && Pawn->GetController() && Pawn->GetController()->IsPlayerController())
		{
			// 오버랩한 플레이어 저장
			LastOverlapPawn = Pawn;

			// 아이템을 주운 것으로 간주
			OnItemPickedUp();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] AI or non-player actor ignored: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPickupSkillItem][OnOverlapBegin] Invalid overlap with: %s"), *OtherActor->GetName());
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
