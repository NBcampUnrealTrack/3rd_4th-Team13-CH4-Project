// TFDSkillEventSubsystem.cpp
#include "Object/SkillItem/Subsystem/TFDSkillEventSubsystem.h"
#include "Engine/World.h"
#include "Character/TFDCharacterBase.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"

void UTFDSkillEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("[TFDSkillEventSubsystem][Initialize] Initialized successfully"));
}

void UTFDSkillEventSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("[TFDSkillEventSubsystem][Deinitialize] Deinitializing..."));

    // 모든 바인딩 해제
    OnSkillItemObtained.Clear();

    Super::Deinitialize();
}

void UTFDSkillEventSubsystem::BroadcastSkillItemObtained(FGameplayTag SkillTag, APawn* TargetPawn)
{
    if (!TargetPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillEventSubsystem][BroadcastSkillItemObtainedToPlayer] TargetPawn is null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[UTFDSkillEventSubsystem][BroadcastSkillItemObtainedToPlayer] Broadcasting skill %s to specific player %s"),
        *SkillTag.ToString(), *TargetPawn->GetName());

    // 타겟 플레이어 정보와 함께 브로드캐스트
    OnSkillItemObtained.Broadcast(SkillTag, TargetPawn);
}

bool UTFDSkillEventSubsystem::AddSkillDirectlyToPlayer(FGameplayTag SkillTag, APawn* TargetPawn, int32 UsageCount)
{
	if (!TargetPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillEventSubsystem][AddSkillDirectlyToPlayer] TargetPawn is null"));
		return false;
	}

	// 서버 권한 체크
	if (!TargetPawn->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillEventSubsystem][AddSkillDirectlyToPlayer] No server authority on TargetPawn"));
		return false;
	}

	// TFDCharacterBase로 캐스팅
	ATFDCharacterBase* TFDCharacter = Cast<ATFDCharacterBase>(TargetPawn);
	if (!TFDCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillEventSubsystem][AddSkillDirectlyToPlayer] TargetPawn is not ATFDCharacterBase"));
		return false;
	}

	// 스킬 매니저 컴포넌트 가져오기
	UTFDSkillManagerComponent* SkillManager = TFDCharacter->GetSkillManagerComponent();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UTFDSkillEventSubsystem][AddSkillDirectlyToPlayer] SkillManager not found on target"));
		return false;
	}

	// 직접 스킬 추가
	SkillManager->ServerAddSkillByTag(SkillTag, UsageCount);

	UE_LOG(LogTemp, Log, TEXT("[UTFDSkillEventSubsystem][AddSkillDirectlyToPlayer] Successfully added skill %s directly to player %s"),
		*SkillTag.ToString(), *TargetPawn->GetName());

	return true;
}

UTFDSkillEventSubsystem* UTFDSkillEventSubsystem::GetSkillEventSubsystem(const UObject* WorldContextObject, bool& bIsValid)
{
    bIsValid = false;
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UTFDSkillEventSubsystem* Subsystem = World->GetSubsystem<UTFDSkillEventSubsystem>();
    bIsValid = (Subsystem != nullptr);
    return Subsystem;
}

UTFDSkillEventSubsystem* UTFDSkillEventSubsystem::GetSkillEventSubsystemSimple(const UObject* WorldContextObject)
{
    bool bIsValid;
    return GetSkillEventSubsystem(WorldContextObject, bIsValid);
}
