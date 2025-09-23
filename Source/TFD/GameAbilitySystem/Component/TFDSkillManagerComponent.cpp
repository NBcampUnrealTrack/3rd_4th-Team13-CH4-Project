// UTFDSkillManagerComponent.cpp
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"

UTFDSkillManagerComponent::UTFDSkillManagerComponent()
{
	// Tick 사용X
	PrimaryComponentTick.bCanEverTick = false;

	// 복제 활성화
	SetIsReplicatedByDefault(true);
}

void UTFDSkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ASC 초기화 시도
	SetupASC();
}

void UTFDSkillManagerComponent::SetupASC()
{
	if (bASCSetup) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][InitializeASC] Owner is not a Pawn."));
		return;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OwnerPawn);
	if (!ASCInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][InitializeASC] Pawn does not implement IAbilitySystemInterface."));
		return;
	}

	ASC = ASCInterface->GetAbilitySystemComponent();
	if (ASC)
	{
		bASCSetup = true;
		SkillSlots.SetNum(MaxSkillSlotCount);

		UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][InitializeASC] ASC initialized successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][InitializeASC] ASC is null even though Pawn implements IAbilitySystemInterface."));
	}
}

bool UTFDSkillManagerComponent::IsASCSetup() const
{
	return bASCSetup;
}

int32 UTFDSkillManagerComponent::GetMaxSlotCount() const
{
	return MaxSkillSlotCount;
}

void UTFDSkillManagerComponent::AddSkill(TSubclassOf<UGameplayAbility> SkillClass, FGameplayTag SkillTag, int32 InitialUsageCount)
{
	// 유효성 검사
	if (!bASCSetup || !ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][AddSkill] ASC not initialized."));
		return;
	}
	if (!SkillClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][AddSkill] SkillClass is null."));
		return;
	}

	// 서버에서만 실행할 코드
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		//=============================================================================
		// 이미 같은 스킬 태그가 있으면 사용 횟수만 누적
		//=============================================================================
		for (FTFDSkillSlot& Slot : SkillSlots)
		{
			if (Slot.SkillTag == SkillTag && Slot.AbilityHandle.IsValid())
			{
				Slot.UsageCount += InitialUsageCount;
				UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][AddSkill] Existing skill %s found. UsageCount = %d"),
					*SkillTag.ToString(), Slot.UsageCount);

				OnSkillChanged.Broadcast(SkillSlots);
				return;
			}
		}

		//=============================================================================
		// 새로운 AbilitySpec 추가
		//=============================================================================
		FGameplayAbilitySpec NewSpec(SkillClass, 1);
		FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(NewSpec);

		FTFDSkillSlot NewSlot;
		NewSlot.AbilityHandle = NewHandle;
		NewSlot.SkillTag = SkillTag;
		NewSlot.UsageCount = InitialUsageCount;

		//=============================================================================
		// 빈 슬롯 찾기
		//=============================================================================
		for (int32 i = 0; i < SkillSlots.Num(); ++i)
		{
			// 슬롯 배열은 이미 SetNum(MaxSkillSlotCount)로 고정 크기이므로,
			// 빈 슬롯은 AbilityHandle이 유효하지 않은 슬롯으로 판단
			if (!SkillSlots[i].AbilityHandle.IsValid())
			{
				SkillSlots[i] = NewSlot;
				UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][AddSkill] Added skill %s at empty slot %d"),
					*SkillTag.ToString(), i);

				OnSkillChanged.Broadcast(SkillSlots);
				return;
			}
		}

		//=============================================================================
		// 빈 슬롯이 없을 경우,
		// 첫 슬롯 제거 후 모든 슬롯 왼쪽으로 이동
		// 새 스킬을 마지막 슬롯에 추가
		//=============================================================================
		if (SkillSlots[0].AbilityHandle.IsValid())
		{// 맨 앞 스킬 제거
			ASC->ClearAbility(SkillSlots[0].AbilityHandle);
		}

		for (int32 i = 1; i < SkillSlots.Num(); ++i)
		{// 앞당기기
			SkillSlots[i - 1] = SkillSlots[i];
		}

		// 마지막 슬롯에 새 스킬 추가
		SkillSlots[SkillSlots.Num() - 1] = NewSlot;

		UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][AddSkill] Replaced oldest skill with %s"), *SkillTag.ToString());

		OnSkillChanged.Broadcast(SkillSlots);
	}

	// 클라이언트에서도 항상 실행할 이펙트나 UI 로직이 있다면 여기서 처리
}

void UTFDSkillManagerComponent::RemoveSkill(FGameplayTag SkillTag)
{
	if (!bASCSetup || !ASC) return;

	// 서버에서만 실행할 코드
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (int32 i = 0; i < SkillSlots.Num(); ++i)
		{
			if (SkillSlots[i].SkillTag == SkillTag && SkillSlots[i].AbilityHandle.IsValid())
			{
				// ASC에서 제거
				ASC->ClearAbility(SkillSlots[i].AbilityHandle);

				// 오른쪽 슬롯들을 왼쪽으로 당김
				for (int32 j = i + 1; j < SkillSlots.Num(); ++j)
				{
					SkillSlots[j - 1] = SkillSlots[j];
				}

				// 마지막 슬롯 초기화
				SkillSlots[SkillSlots.Num() - 1] = FTFDSkillSlot();

				UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][RemoveSkill] Skill %s removed."), *SkillTag.ToString());

				OnSkillChanged.Broadcast(SkillSlots);
				return;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][RemoveSkill] Skill %s not found."), *SkillTag.ToString());
	}

	// 클라이언트에서도 항상 실행할 이펙트나 UI 로직이 있다면 여기서 처리
}

FTFDSkillSlot UTFDSkillManagerComponent::GetSkillAt(int32 SlotIndex) const
{
	if (SkillSlots.IsValidIndex(SlotIndex))
	{
		return SkillSlots[SlotIndex];
	}

	UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][GetSkillAt] Invalid slot index [%d]"), SlotIndex);

	// 기본 생성된 빈 슬롯 반환
	return FTFDSkillSlot();
}

void UTFDSkillManagerComponent::UseSkillAtSlot(int32 SlotIndex)
{
	if (!SkillSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Invalid SlotIndex %d"), SlotIndex);
		return;
	}
	if (!bASCSetup || !ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] ASC is not initialized."));
		return;
	}

	// 서버에서만 실행할 코드
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FTFDSkillSlot& Slot = SkillSlots[SlotIndex];

		if (!Slot.AbilityHandle.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Slot %d is empty"), SlotIndex);
			return;
		}

		// 실제 어빌리티 발동
		const bool bSuccess = ASC->TryActivateAbility(Slot.AbilityHandle);

		if (bSuccess)
		{
			Slot.UsageCount--;

			UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Used skill [%s] in slot %d. Remaining count: %d"),
				*Slot.SkillTag.ToString(), SlotIndex, Slot.UsageCount);

			if (Slot.UsageCount <= 0)
			{
				RemoveSkill(Slot.SkillTag);
			}
			else
			{
				OnSkillChanged.Broadcast(SkillSlots);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Failed to activate ability [%s] in slot %d"),
				*Slot.SkillTag.ToString(), SlotIndex);
		}
	}

	// 클라이언트에서도 항상 실행할 이펙트나 UI 로직이 있다면 여기서 처리
}

void UTFDSkillManagerComponent::OnRep_SkillSlots()
{
	OnSkillChanged.Broadcast(SkillSlots);
}

void UTFDSkillManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFDSkillManagerComponent, SkillSlots);
}