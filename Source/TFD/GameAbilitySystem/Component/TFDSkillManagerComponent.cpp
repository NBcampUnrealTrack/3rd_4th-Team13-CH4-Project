// UTFDSkillManagerComponent.cpp
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Object/SkillItem/Subsystem/TFDSkillEventSubsystem.h" // 서브시스템 헤더 추가
#include "Controller/TFDAIController.h"

/*
	이중 검증

	UseSkillAtSlot() 호출
		↓
	1 서버 쿨다운 체크 (ServerCooldownEndTimes) - 이전 쿨다운 확인
		↓
	2 스팸 방지 체크 (LastSkillUseTimes) - 0.1초 간격
		↓
	3 GAS TryActivateAbility() 호출 (GameplayEffect 쿨다운)
		↓
	4 어빌리티 내부에서 CommitAbility() 실행
		↓	GAS 쿨다운 체크
		↓	Cost 체크
		↓
	5 CommitAbility()가 쿨다운 GameplayEffect 적용
		↓
	6. LastSkillUseTimes 업데이트
		↓
	7 [비동기] HandleEffectAdded 자동 호출 (ServerCooldownEndTimes 업데이트, UI 쿨다운 정보 업데이트)
*/

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

	// 게임 내 모든 아이템을 찾아 이벤트 구독
	SubscribeToSkillItemEvents();
}

void UTFDSkillManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 서브시스템 이벤트 구독 해제
	UnsubscribeFromSkillItemEvents();

	//ASC 델리게이트 구독 해제
	if (EffectAddedHandle.IsValid())
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.Remove(EffectAddedHandle);
		EffectAddedHandle.Reset();
	}

	if (EffectRemovedHandle.IsValid())
	{
		ASC->OnAnyGameplayEffectRemovedDelegate().Remove(EffectRemovedHandle);
		EffectRemovedHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
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

		EffectAddedHandle = ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UTFDSkillManagerComponent::HandleEffectAdded);
		EffectRemovedHandle = ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UTFDSkillManagerComponent::HandleEffectRemoved);

		UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][InitializeASC] ASC initialized successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillManagerComponent][InitializeASC] ASC is null even though Pawn implements IAbilitySystemInterface."));
	}
}

void UTFDSkillManagerComponent::HandleEffectAdded(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	if (!InASC) return;

	// 태그 확인
	FGameplayTagContainer EffectTags;
	Spec.GetAllGrantedTags(EffectTags);

	// 어떤 슬롯이 해당 쿨다운인지 찾아서 업데이트
	//for (FTFDSkillSlot& Slot : SkillSlots)
	for (int32 i = 0; i < SkillSlots.Num(); ++i)
	{
		//if (!Slot.AbilityHandle.IsValid()) continue;
		FTFDSkillSlot& Slot = SkillSlots[i];
		if (!Slot.AbilityHandle.IsValid()) continue;

		FGameplayAbilitySpec* AbilitySpec = InASC->FindAbilitySpecFromHandle(Slot.AbilityHandle);
		if (!AbilitySpec || !AbilitySpec->Ability) continue;

		const FGameplayTagContainer* CooldownTags = AbilitySpec->Ability->GetCooldownTags();
		if (!CooldownTags || CooldownTags->Num() == 0) continue;

		if (EffectTags.HasAny(*CooldownTags))
		{
			// Duration / Remaining 계산
			const FActiveGameplayEffect* ActiveEffect = InASC->GetActiveGameplayEffect(Handle);
			if (ActiveEffect)
			{
				float Duration = ActiveEffect->GetDuration();
				//float CurrentTime = GetWorld()->GetTimeSeconds();
				// 수정: 서버 시간 사용
				float ServerTime = InASC->GetWorld()->GetTimeSeconds();

				// UI용 쿨다운 정보
				Slot.CooldownDuration = Duration;
				//Slot.CooldownStartTime = CurrentTime;
				Slot.CooldownStartTime = ServerTime;

				// 서버 쿨다운 검증용 (추가)
				if (GetOwner()->HasAuthority())
				{
					//ServerCooldownEndTimes.Add(i, CurrentTime + Duration);
					// 수정: FindOrAdd 사용
					ServerCooldownEndTimes.FindOrAdd(i) = ServerTime + Duration;

					// 수정: ServerTime과 EndTime 추가
					UE_LOG(LogTemp, Log,
						TEXT("[UTFDSkillManagerComponent][HandleEffectAdded][Server] Cooldown started for slot %d: Duration=%.2fs, ServerTime=%.2f, EndTime=%.2f"),
						i, Duration, ServerTime, ServerTime + Duration);
				}
			}

			break; // 해당 슬롯 찾았으면 끝
		}
	}

	// UI 갱신
	OnSkillChanged.Broadcast(SkillSlots);
}

void UTFDSkillManagerComponent::HandleEffectRemoved(const FActiveGameplayEffect& ActiveEffect)
{
	// 태그 가져오기
	FGameplayTagContainer EffectTags;
	ActiveEffect.Spec.GetAllGrantedTags(EffectTags);

	//for (FTFDSkillSlot& Slot : SkillSlots)
	for (int32 i = 0; i < SkillSlots.Num(); ++i)
	{
		//if (!Slot.AbilityHandle.IsValid()) continue;
		FTFDSkillSlot& Slot = SkillSlots[i];
		if (!Slot.AbilityHandle.IsValid()) continue;

		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Slot.AbilityHandle);
		if (!Spec || !Spec->Ability) continue;

		const FGameplayTagContainer* CooldownTags = Spec->Ability->GetCooldownTags();
		if (!CooldownTags || CooldownTags->Num() == 0) continue;

		if (EffectTags.HasAny(*CooldownTags))
		{
			// UI용 쿨다운 초기화
			Slot.CooldownDuration = 0.f;
			Slot.CooldownStartTime = 0.f;

			// 서버 쿨다운 제거 (추가)
			if (GetOwner()->HasAuthority())
			{
				ServerCooldownEndTimes.Remove(i);

				// 추가: LastSkillUseTimes도 정리
				LastSkillUseTimes.Remove(i);

				UE_LOG(LogTemp, Log,
					TEXT("[UTFDSkillManagerComponent][HandleEffectRemoved][Server] Cooldown ended for slot %d"), i);
			}
			break;
		}
	}

	// UI 갱신
	OnSkillChanged.Broadcast(SkillSlots);
}

void UTFDSkillManagerComponent::ApplyExistingCooldownToSlot(int32 SlotIndex, TSubclassOf<UGameplayAbility> SkillClass)
{
	if (!SkillSlots.IsValidIndex(SlotIndex) || !ASC || !SkillClass) return;

	UGameplayAbility* AbilityCDO = SkillClass->GetDefaultObject<UGameplayAbility>();
	if (!AbilityCDO) return;

	const FGameplayTagContainer* CooldownTags = AbilityCDO->GetCooldownTags();
	if (!CooldownTags || CooldownTags->Num() == 0) return;

	// ASC에서 해당 태그와 매칭되는 쿨다운 효과 검색
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
	TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);

	// 수정: 서버 시간 사용
	float ServerTime = ASC->GetWorld()->GetTimeSeconds();

	for (FActiveGameplayEffectHandle ActiveHandle : ActiveEffects)
	{
		const FActiveGameplayEffect* ActiveEffect = ASC->GetActiveGameplayEffect(ActiveHandle);
		if (!ActiveEffect) continue;

		float Duration = ActiveEffect->GetDuration();
		//float TimeRemaining = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		float TimeRemaining = ActiveEffect->GetTimeRemaining(ServerTime);

		if (Duration > 0.f && TimeRemaining > 0.f)
		{
			SkillSlots[SlotIndex].CooldownDuration = Duration;
			//SkillSlots[SlotIndex].CooldownStartTime = GetWorld()->GetTimeSeconds() - (Duration - TimeRemaining);
			SkillSlots[SlotIndex].CooldownStartTime = ServerTime - (Duration - TimeRemaining);
			return; // 첫 번째 매칭만 적용
		}
	}
}

bool UTFDSkillManagerComponent::IsASCSetup() const { return bASCSetup; }

bool UTFDSkillManagerComponent::IsSkillOnServerCooldown(int32 SlotIndex) const
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (const float* CooldownEnd = ServerCooldownEndTimes.Find(SlotIndex))
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		bool bIsOnCooldown = CurrentTime < *CooldownEnd;

		if (bIsOnCooldown)
		{
			float Remaining = *CooldownEnd - CurrentTime;
			UE_LOG(LogTemp, Verbose, TEXT("[UTFDSkillManagerComponent][IsSkillOnServerCooldown] Slot %d on cooldown: %.2fs remaining"),
				SlotIndex, Remaining);
		}

		return bIsOnCooldown;
	}

	return false;
}

int32 UTFDSkillManagerComponent::GetMaxSlotCount() const { return MaxSkillSlotCount; }

void UTFDSkillManagerComponent::SubscribeToSkillItemEvents()
{
	// 디버그 로그: 누가 호출했는지 확인
	UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][SubscribeToSkillItemEvents] Called! Owner: %s | Role: %s"),
		*GetOwner()->GetName(),
		*UEnum::GetValueAsString(GetOwner()->GetLocalRole()));

	// 중복 구독 방지
	if (bIsSubscribedToSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][SubscribeToSkillItemEvents] Already subscribed to subsystem"));
		return;
	}

	// 플레이어 캐릭터만 구독 (AI 제외)
	// 주석 처리한 아래 조건으로 하면 안되는데 왜 그럴까?
	//APawn* OwnerPawn = Cast<APawn>(GetOwner());
	//if (!OwnerPawn || !OwnerPawn->GetController() || !OwnerPawn->GetController()->IsPlayerController())
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController && OwnerController->IsA(AAIController::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][SubscribeToSkillItemEvents] Skipping subscription - not player controller"));
		return;
	}

	UTFDSkillEventSubsystem* SkillEventSubsystem = UTFDSkillEventSubsystem::GetSkillEventSubsystemSimple(this);
	if (!SkillEventSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][SubscribeToSkillItemEvents] Failed to get SkillEventSubsystem"));
		return;
	}

	// 기존 바인딩 제거 후 새로 바인딩
	SkillEventSubsystem->OnSkillItemObtained.RemoveDynamic(this, &UTFDSkillManagerComponent::OnSkillItemObtained);
	SkillEventSubsystem->OnSkillItemObtained.AddDynamic(this, &UTFDSkillManagerComponent::OnSkillItemObtained);

	// 바인딩 후 상태를 업데이트
	bIsSubscribedToSubsystem = true;
	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][SubscribeToSkillItemEvents] Successfully subscribed to skill event subsystem"));
}

void UTFDSkillManagerComponent::UnsubscribeFromSkillItemEvents()
{
	if (!bIsSubscribedToSubsystem)
	{
		return;
	}

	UTFDSkillEventSubsystem* SkillEventSubsystem = UTFDSkillEventSubsystem::GetSkillEventSubsystemSimple(this);
	if (SkillEventSubsystem)
	{
		// RemoveDynamic 호출: 바인딩되지 않았더라도 안전하게 처리됨
		SkillEventSubsystem->OnSkillItemObtained.RemoveDynamic(this, &UTFDSkillManagerComponent::OnSkillItemObtained);
		bIsSubscribedToSubsystem = false;
		UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][UnsubscribeFromSkillItemEvents] Unsubscribed from skill event subsystem"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][UnsubscribeFromSkillItemEvents] Failed to get SkillEventSubsystem"));
	}
}

void UTFDSkillManagerComponent::OnSkillItemObtained(FGameplayTag SkillTag, APawn* TargetPawn)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] Owner is not a pawn"));
		return;
	}

	// 타겟 플레이어 필터링
	if (TargetPawn && OwnerPawn != TargetPawn)
	{// 이 스킬 이벤트는 다른 플레이어를 대상으로 하므로 무시
		UE_LOG(LogTemp, Verbose, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] Skill event ignored - not target player (Owner: %s, Target: %s)"),
			*OwnerPawn->GetName(), *TargetPawn->GetName());
		return;
	}

	if (!TargetPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] Received skill event with no target player - using legacy mode"));
	}

	// 플레이어 컨트롤러 체크
	if (!OwnerPawn->GetController() || !OwnerPawn->GetController()->IsPlayerController())
	{
		UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] Ignoring skill for non-player"));
		return;
	}

	// PlayerState 유효성 체크
	if (!OwnerPawn->GetPlayerState())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] PlayerState is null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][OnSkillItemObtained] Adding skill %s to player %s"), *SkillTag.ToString(), *OwnerPawn->GetPlayerState()->GetName());

	// 스킬 추가
	HandleSkillItemObtained(SkillTag);
}

void UTFDSkillManagerComponent::HandleSkillItemObtained(FGameplayTag SkillTag)
{
	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][HandleSkillItemObtained] Received skill item: %s"), *SkillTag.ToString());

	const int32 DefaultUsageCount = 1;

	if (GetOwner() && GetOwner()->HasAuthority())
	{// 서버 권한이 있는 경우 직접 추가
		AddSkillByTag(SkillTag, DefaultUsageCount);
	}
	else
	{// 클라이언트는 서버 RPC 호출
		ServerAddSkillByTag(SkillTag, DefaultUsageCount);
	}

	//// 스킬 태그별 추가 처리
	//if (SkillTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Thief.Invisibility"))))
	//{
	//	HandleInvisibilitySkill();
	//}
	//else if (SkillTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Mage.Fireball"))))
	//{
	//	HandleFireballSkill();
	//}
	//// 다른 스킬들에 대한 처리...
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

				ApplyExistingCooldownToSlot(i, SkillClass);

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

			// 첫 번째 슬롯의 쿨다운 데이터 제거
			ServerCooldownEndTimes.Remove(0);
			LastSkillUseTimes.Remove(0);
		}

		for (int32 i = 1; i < SkillSlots.Num(); ++i)
		{// 앞당기기
			SkillSlots[i - 1] = SkillSlots[i];

			// 쿨다운 데이터도 인덱스 조정
			if (ServerCooldownEndTimes.Contains(i))
			{
				float CooldownEnd = ServerCooldownEndTimes[i];
				ServerCooldownEndTimes.Remove(i);
				//ServerCooldownEndTimes.Add(i - 1, CooldownEnd);
				ServerCooldownEndTimes.FindOrAdd(i - 1) = CooldownEnd;
			}

			if (LastSkillUseTimes.Contains(i))
			{
				float LastUseTime = LastSkillUseTimes[i];
				LastSkillUseTimes.Remove(i);
				//LastSkillUseTimes.Add(i - 1, LastUseTime);
				LastSkillUseTimes.FindOrAdd(i - 1) = LastUseTime;
			}
		}

		// 마지막 슬롯에 새 스킬 추가
		int32 LastIndex = SkillSlots.Num() - 1;
		SkillSlots[LastIndex] = NewSlot;

		// 마지막 슬롯의 남은 쿨다운 데이터 제거
		ServerCooldownEndTimes.Remove(LastIndex);
		LastSkillUseTimes.Remove(LastIndex);

		UE_LOG(LogTemp, Log, TEXT("[SkillManagerComponent][AddSkill] Replaced oldest skill with %s"), *SkillTag.ToString());
		
		ApplyExistingCooldownToSlot(LastIndex, SkillClass);

		OnSkillChanged.Broadcast(SkillSlots);
		// 서버가 로컬 플레이어(리슨 서버의 호스트)인 경우 직접 브로드캐스트
		//APawn* OwnerPawn = Cast<APawn>(GetOwner());
		//if (OwnerPawn && OwnerPawn->IsLocallyControlled())
		//{
		//	OnSkillChanged.Broadcast(SkillSlots);
		//}
	}

	// 클라이언트에서도 항상 실행할 이펙트나 UI 로직이 있다면 여기서 처리
}

void UTFDSkillManagerComponent::AddSkillByTag(FGameplayTag SkillTag, int32 UsageCount)
{
	if (!bASCSetup || !ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][AddSkillByTag] ASC not setup"));
		return;
	}

	if (!SkillTagToClassMap.Contains(SkillTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][AddSkillByTag] No SkillClass found for Tag: %s"), *SkillTag.ToString());
		return;
	}

	TSubclassOf<UGameplayAbility> SkillClass = SkillTagToClassMap[SkillTag];
	if (!SkillClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillManagerComponent][AddSkillByTag] SkillClass is null for Tag: %s"), *SkillTag.ToString());
		return;
	}

	// 기존 AddSkill 호출
	AddSkill(SkillClass, SkillTag, UsageCount);
}

void UTFDSkillManagerComponent::ServerAddSkillByTag_Implementation(FGameplayTag SkillTag, int32 UsageCount)
{
	AddSkillByTag(SkillTag, UsageCount);
}

bool UTFDSkillManagerComponent::ServerAddSkillByTag_Validate(FGameplayTag SkillTag, int32 UsageCount)
{	
	if (!SkillTag.IsValid())
	{// 태그 유효성 체크
		UE_LOG(LogTemp, Error, TEXT("[UTFDSkillManagerComponent][ServerAddSkillByTag_Validate][AntiCheat] Invalid skill tag"));
		return false;
	}
	
	if (UsageCount <= 0 || UsageCount > 100)
	{// 사용 횟수 범위 체크 (비정상적인 값 차단)
		UE_LOG(LogTemp, Error, TEXT("[UTFDSkillManagerComponent][ServerAddSkillByTag_Validate][AntiCheat] Invalid usage count: %d"), UsageCount);
		return false;
	}
	
	if (!SkillTagToClassMap.Contains(SkillTag))
	{// 매핑된 스킬인지 체크
		UE_LOG(LogTemp, Error, TEXT("[UTFDSkillManagerComponent][ServerAddSkillByTag_Validate][AntiCheat] Unmapped skill tag: %s"), *SkillTag.ToString());
		return false;
	}

	return true;
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

				// 서버 쿨다운 데이터 정리
				ServerCooldownEndTimes.Remove(i);
				LastSkillUseTimes.Remove(i);

				// 오른쪽 슬롯들을 왼쪽으로 당김
				for (int32 j = i + 1; j < SkillSlots.Num(); ++j)
				{
					SkillSlots[j - 1] = SkillSlots[j];

					// 쿨다운 데이터도 인덱스 조정
					if (ServerCooldownEndTimes.Contains(j))
					{
						float CooldownEnd = ServerCooldownEndTimes[j];
						ServerCooldownEndTimes.Remove(j);
						//ServerCooldownEndTimes.Add(j - 1, CooldownEnd);
						ServerCooldownEndTimes.FindOrAdd(j - 1) = CooldownEnd;
					}

					if (LastSkillUseTimes.Contains(j))
					{
						float LastUseTime = LastSkillUseTimes[j];
						LastSkillUseTimes.Remove(j);
						//LastSkillUseTimes.Add(j - 1, LastUseTime);
						LastSkillUseTimes.FindOrAdd(j - 1) = LastUseTime;
					}
				}

				// 마지막 슬롯 초기화
				int32 LastIndex = SkillSlots.Num() - 1;
				SkillSlots[LastIndex] = FTFDSkillSlot();
				ServerCooldownEndTimes.Remove(LastIndex);
				LastSkillUseTimes.Remove(LastIndex);

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

		// 1. 서버 쿨다운 체크 (GAS 우회 방지)
		if (IsSkillOnServerCooldown(SlotIndex))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot][AntiCheat] Slot %d on server cooldown (possible hack)"), SlotIndex);
			return;
		}

		// 2. 스팸 방지
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (const float* LastUseTime = LastSkillUseTimes.Find(SlotIndex))
		{
			float TimeSince = CurrentTime - *LastUseTime;
			if (TimeSince < MinSkillInterval)
			{
				UE_LOG(LogTemp, Warning,
					TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot][AntiCheat] Skill spam detected! Slot: %d, Interval: %.3fs"),
					SlotIndex, TimeSince);
				return;
			}
		}

		// 3. GAS 어빌리티 활성화 시도 (CommitAbility 포함)
		// 실제 어빌리티 발동
		const bool bSuccess = ASC->TryActivateAbility(Slot.AbilityHandle);

		if (bSuccess)
		{
			// 4. 스팸 방지용 시간 기록
			// ServerCooldownEndTimes는 HandleEffectAdded에서 자동으로 설정됨
			//LastSkillUseTimes.Add(SlotIndex, CurrentTime);

			// 수정: FindOrAdd 사용
			LastSkillUseTimes.FindOrAdd(SlotIndex) = CurrentTime;

			Slot.UsageCount--;

			UE_LOG(LogTemp, Log,
				TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Used skill [%s] in slot %d. Remaining count: %d"),
				*Slot.SkillTag.ToString(), SlotIndex, Slot.UsageCount);

			if (Slot.UsageCount <= 0)
			{
				//RemoveSkill(Slot.SkillTag);

				// 스킬 태그 저장 (타이머 람다에서 사용)
				FGameplayTag SkillTagToRemove = Slot.SkillTag;

				// 즉시 제거하지 않고 0.1초 지연
				FTimerHandle RemoveTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					RemoveTimerHandle,
					[this, SkillTagToRemove]()
					{
						RemoveSkill(SkillTagToRemove);
						UE_LOG(LogTemp, Log,
							TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Delayed removal of skill: %s"),
							*SkillTagToRemove.ToString());
					},
					0.1f, // 0.1초 지연
					false
				);

				// UI 업데이트는 즉시 (카운트 0 표시)
				OnSkillChanged.Broadcast(SkillSlots);
			}
			else
			{
				OnSkillChanged.Broadcast(SkillSlots);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[UTFDSkillManagerComponent][UseSkillAtSlot] Failed to activate ability [%s] in slot %d"),
				*Slot.SkillTag.ToString(), SlotIndex);
		}
	}

	// 클라이언트에서도 항상 실행할 이펙트나 UI 로직이 있다면 여기서 처리
}

void UTFDSkillManagerComponent::OnRep_SkillSlots()
{
	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][OnRep_SkillSlots] Replication received. Broadcasting to UI. Slot count: %d"), SkillSlots.Num());

	// 각 슬롯 상태 로깅
	for (int32 i = 0; i < SkillSlots.Num(); ++i)
	{
		const FTFDSkillSlot& Slot = SkillSlots[i];
		if (Slot.SkillTag.IsValid() && Slot.AbilityHandle.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][OnRep_SkillSlots] Slot %d: %s (Count: %d)"), i, *Slot.SkillTag.ToString(), Slot.UsageCount);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][OnRep_SkillSlots] Slot %d: Empty"), i);
		}
	}

	OnSkillChanged.Broadcast(SkillSlots);
}

void UTFDSkillManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 각 플레이어의 스킬은 해당 플레이어(Owner)에게만 리플리케이션
	//DOREPLIFETIME(UTFDSkillManagerComponent, SkillSlots);
	DOREPLIFETIME_CONDITION(UTFDSkillManagerComponent, SkillSlots, COND_OwnerOnly);
}

void UTFDSkillManagerComponent::InitializeSkillSlots()
{
	// 이전에 설정된 슬롯이 있다면 초기화
	SkillSlots.Empty();

	// MaxSlots 크기만큼 슬롯 초기화
	SkillSlots.SetNum(MaxSkillSlotCount);
}

//void UTFDSkillManagerComponent::HandleInvisibilitySkill()
//{
//	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][HandleInvisibilitySkill] Processing Invisibility Skill"));
//
//	// 예: 특별한 효과, UI 업데이트, 사운드 재생 등
//}
//
//void UTFDSkillManagerComponent::HandleFireballSkill()
//{
//	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillManagerComponent][HandleFireballSkill] Processing Fireball Skill"));
//
//	// 파이어볼 스킬 추가 처리 로직
//}