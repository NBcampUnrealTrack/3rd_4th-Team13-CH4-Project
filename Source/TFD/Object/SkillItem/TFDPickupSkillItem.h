// TFDPickupSkillItem.h
/*
	
*/
#pragma once

#include "CoreMinimal.h"
#include "Object/TFDBaseObject.h"
#include "GameplayTagContainer.h" // GameplayTag 사용을 위해 추가
#include "TFDPickupSkillItem.generated.h"

// 아이템 획득 이벤트 델리게이트
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillItemObtained, FGameplayTag, SkillTag);

UCLASS()
class TFD_API ATFDPickupSkillItem : public ATFDBaseObject
{
	GENERATED_BODY()

public:
	ATFDPickupSkillItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapSkill(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:
	// 아이템 획득 시 호출될 함수
	void OnItemPickedUp();

	// 아이템 획득 후 일정 시간 후 파괴
	void DelayedDestroy();

	//// 아이템이 획득되었을 때 발생하는 이벤트
	//UPROPERTY(BlueprintAssignable, Category = "TFD|PickupSkillItem")
	//FOnSkillItemObtained OnSkillItemObtained;

	// 스킬 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TFD|PickupSkillItem")
	FGameplayTag PickupSkillItemSkillTag;

	// 새로운 브로드캐스트 함수 (타겟 플레이어 지정)
	void BroadcastSkillItemObtainedToPlayer(FGameplayTag SkillTag, APawn* TargetPawn);

	//// RPC - 클라이언트에서 서버로 아이템 획득 요청
	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerRequestPickup(APawn* RequestingPawn);
	////void ServerRequestPickup_Implementation(APawn* RequestingPawn);
	////bool ServerRequestPickup_Validate(APawn* RequestingPawn);

private:
	bool bIsPickedUp;			// 아이템 획득 상태 체크
	bool bIsDestroyed;			// 아이템 파괴 여부 체크
	bool bIsProcessingPickup;	// 처리 중 플래그

	// 아이템 파괴를 위한 타이머 핸들
	FTimerHandle DestroyTimerHandle;

	// 마지막으로 오버랩한 플레이어 저장
	UPROPERTY()
	APawn* LastOverlapPawn = nullptr;

};
