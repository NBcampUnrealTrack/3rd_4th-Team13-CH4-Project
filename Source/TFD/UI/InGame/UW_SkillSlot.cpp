// UW_SkillSlot.cpp
#include "UI/InGame/UW_SkillSlot.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

//void UUW_SkillSlot::UpdateSkillSlot(int32 SlotIndex, const FTFDSkillSlot& SkillSlot)
//{
//	CurrentSkillSlot = SkillSlot;
//
//	// UI 표시용으로 1번부터 시작
//	int32 UISlotIndex = SlotIndex + 1;
//
//	// 스킬 태그가 유효한지 확인하고 빈 슬롯 처리
//	FString SlotText;
//	if (SkillSlot.SkillTag.IsValid() && SkillSlot.AbilityHandle.IsValid())
//	{// 유효한 스킬이 있는 경우
//		// 태그에서 마지막 부분만 추출 (예: "Invisibility" 또는 "Teleport")
//		FString SkillTagString = SkillSlot.SkillTag.ToString();
//		FString ShortSkillTag = SkillTagString.RightChop(SkillTagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);
//
//		//SlotText = FString::Printf(TEXT("%d번: %s (사용횟수: %d)"), UISlotIndex, *SkillSlot.SkillTag.ToString(), SkillSlot.UsageCount);
//		SlotText = FString::Printf(TEXT("%d번: %s (사용횟수: %d)"), UISlotIndex, *ShortSkillTag, CurrentSlot.UsageCount);
//	}
//	else
//	{// 빈 슬롯인 경우
//		SlotText = FString::Printf(TEXT("%d번: 빈 슬롯"), UISlotIndex);
//	}
//
//	// 스킬 정보를 업데이트 (기존 텍스트 대신 새로운 텍스트로 교체)
//	if (Skillinfo)
//	{
//		Skillinfo->SetText(FText::FromString(SlotText));
//		UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][UpdateSkillSlot] Updated slot %d (UI: %d): %s"), SlotIndex, UISlotIndex, *SlotText);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[UW_SkillSlot][UpdateSkillSlot] Skillinfo is null!"));
//	}
//}

void UUW_SkillSlot::OnSkillChanged(const TArray<FTFDSkillSlot>& SkillSlots)
{
	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][OnSkillChanged] Skill slots changed. Count: %d"), SkillSlots.Num());

	// 네트워크 역할 정보 추가
	APlayerController* PC = GetOwningPlayer();
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	FString RoleString = PlayerPawn ? UEnum::GetValueAsString(PlayerPawn->GetLocalRole()) : TEXT("Unknown");
	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][OnSkillChanged] Skill slots changed. Count: %d, Role: %s"), SkillSlots.Num(), *RoleString);

	// 텍스트 초기화
	if (Skillinfo)
	{
		Skillinfo->SetText(FText::FromString(TEXT("")));
	}

	// 모든 스킬 슬롯 정보를 하나의 텍스트로 합치기
	FString AllSlotsText = TEXT("");

	for (int32 i = 0; i < SkillSlots.Num(); ++i)
	{
		const FTFDSkillSlot& CurrentSlot = SkillSlots[i];

		// 스킬 태그가 유효한지 확인하고 빈 슬롯 처리
		FString SlotText;

		// UI 표시용으로 1번부터 시작
		int32 UISlotIndex = i + 1;

		if (CurrentSlot.SkillTag.IsValid() && CurrentSlot.AbilityHandle.IsValid())
		{// 유효한 스킬이 있는 경우
			// 태그에서 마지막 부분만 추출 (예: "Invisibility" 또는 "Teleport")
			FString SkillTagString = CurrentSlot.SkillTag.ToString();
			FString ShortSkillTag = SkillTagString.RightChop(SkillTagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);

			//SlotText = FString::Printf(TEXT("%d번: %s (사용횟수: %d)"), UISlotIndex, *CurrentSlot.SkillTag.ToString(), CurrentSlot.UsageCount);
			SlotText = FString::Printf(TEXT("%d번: %s (사용횟수: %d)"), UISlotIndex, *ShortSkillTag, CurrentSlot.UsageCount);
		}
		else
		{// 빈 슬롯인 경우
			SlotText = FString::Printf(TEXT("%d번: 빈 슬롯"), UISlotIndex);
		}

		// 전체 텍스트에 추가
		if (i > 0)
		{
			AllSlotsText += TEXT("\n");
		}
		AllSlotsText += SlotText;

		UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][OnSkillChanged] Slot %d (UI: %d): %s"), i, UISlotIndex, *SlotText);
	}

	// 최종 텍스트 설정
	if (Skillinfo)
	{
		Skillinfo->SetText(FText::FromString(AllSlotsText));
		UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][OnSkillChanged] Final text set: %s"), *AllSlotsText);
	}
}
void UUW_SkillSlot::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][NativeConstruct] Widget constructed"));

	// 네트워크 역할 정보 로깅
	APlayerController* PC = GetOwningPlayer();
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	FString RoleString = PlayerPawn ? UEnum::GetValueAsString(PlayerPawn->GetLocalRole()) : TEXT("Unknown");
	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][NativeConstruct] Widget constructed on %s"), *RoleString);

	// Skillinfo 위젯이 바인딩되었는지 확인
	if (!Skillinfo)
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_SkillSlot][NativeConstruct] Skillinfo TextBlock is not bound! Check widget blueprint."));
		return;
	}

	// 초기 텍스트 설정
	Skillinfo->SetText(FText::FromString(TEXT("스킬 슬롯 초기화 중...")));
	if (!Skillinfo)
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_SkillSlot][NativeConstruct] Skillinfo TextBlock is not bound! Check widget blueprint."));
		return;
	}

	// PlayerController 가져오기
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_SkillSlot][NativeConstruct] Failed to get PlayerController"));
		return;
	}

	// PlayerController의 Pawn 가져오기
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UW_SkillSlot][NativeConstruct] PlayerController has no Pawn yet"));

		// Pawn이 아직 없다면 타이머로 다시 시도
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_RetryBinding,
			this,
			&UUW_SkillSlot::TryBindToSkillManager,
			0.1f,
			true  // 반복
		);
		return;
	}

	// 스킬 매니저 컴포넌트 찾기
	UTFDSkillManagerComponent* SkillManager = PlayerPawn->FindComponentByClass<UTFDSkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_SkillSlot][NativeConstruct] No SkillManagerComponent found on PlayerPawn"));
		return;
	}

	// 이벤트 바인딩
	SkillManager->OnSkillChanged.AddDynamic(this, &UUW_SkillSlot::OnSkillChanged);
	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][NativeConstruct] Successfully bound to SkillManager"));

	// 현재 스킬 상태로 초기화
	const TArray<FTFDSkillSlot>& CurrentSlots = SkillManager->GetAllSkills();
	OnSkillChanged(CurrentSlots);
}

void UUW_SkillSlot::TryBindToSkillManager()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{// 아직 Pawn이 없음, 계속 대기
		return;
	}

	// 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RetryBinding);

	// 스킬 매니저 컴포넌트 찾기
	UTFDSkillManagerComponent* SkillManager = PlayerPawn->FindComponentByClass<UTFDSkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_SkillSlot][TryBindToSkillManager] No SkillManagerComponent found on PlayerPawn"));
		return;
	}

	// 이벤트 바인딩
	SkillManager->OnSkillChanged.AddDynamic(this, &UUW_SkillSlot::OnSkillChanged);
	UE_LOG(LogTemp, Log, TEXT("[UW_SkillSlot][TryBindToSkillManager] Successfully bound to SkillManager"));

	// 현재 스킬 상태로 초기화
	const TArray<FTFDSkillSlot>& CurrentSlots = SkillManager->GetAllSkills();
	OnSkillChanged(CurrentSlots);
}