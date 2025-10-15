#include "Character/TFDCharacterBase.h"
#include "GameAbilitySystem/Attibute/TFDAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "TFDPlayerDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerState/TFDPlayerState.h"
#include "GameState/TFDGameState.h"
#include "Net/UnrealNetwork.h"

ATFDCharacterBase::ATFDCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UTFDAttributeSet>(TEXT("AttributeSet"));

	// 스킬 매니저 컴포넌트 생성
	SkillManagerComponent = CreateDefaultSubobject<UTFDSkillManagerComponent>(TEXT("SkillManagerComponent"));
}

// Called when the game starts or when spawned
void ATFDCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	BaseSetting();

	// 스킬 시스템 관련
	if (SkillManagerComponent && SkillManagerComponent->IsASCSetup())
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDCharacterBase][BeginPlay] SkillManagerComponent is initialized with ASC."));
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT("[ATFDCharacterBase][BeginPlay] SkillManagerComponent is NOT initialized or ASC is missing."));
	}

	// 캐릭터별 머터리얼 설정.
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	UTFDPlayerDataAsset* Data = CharacterData.Get();
	if ( Data && Data->MaterialInstance && SkeletalMesh)
	{
		SkeletalMesh->SetMaterial(0, CharacterData->MaterialInstance);
	}

}

void ATFDCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATFDCharacterBase, CharacterData, COND_InitialOnly);
}

void ATFDCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (APlayerState* PS = GetPlayerState())
	{
		if (AbilitySystemComponent)
		{
			// ASC의 Owner는 PlayerState, Avatar는 이 캐릭터(Pawn)
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
	}

	// 스킬 시스템 관련
	if (SkillManagerComponent)
	{
		SkillManagerComponent->SetupASC();
		UE_LOG(LogTemp, Log, TEXT("[ATFDCharacterBase][OnRep_PlayerState] SkillManagerComponent initialized."));
	}
}

UAbilitySystemComponent* ATFDCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATFDCharacterBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ATFDCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AttributeSet == nullptr)
	{
		AttributeSet = NewObject<UTFDAttributeSet>();
	}
	// 서버일 때만 실행
	if (HasAuthority())
	{
		if (APlayerState* PS = GetPlayerState())
		{
			if (AbilitySystemComponent)
			{
				AbilitySystemComponent->InitAbilityActorInfo(PS, this);
			}
		}
		else 
		{
			if (AbilitySystemComponent)
			{
				AbilitySystemComponent->InitAbilityActorInfo(NewController, this);
			}
		}
		SetDAPlayerStat();
	}

	// 스킬 시스템 관련
	if (SkillManagerComponent)
	{
		SkillManagerComponent->SetupASC();
		UE_LOG(LogTemp, Log, TEXT("[ATFDCharacterBase][PossessedBy] SkillManagerComponent initialized."));
	}
}

void ATFDCharacterBase::ServerUseSkillAtSlot_Implementation(int32 SlotIndex)
{
	if (!SkillManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDCharacterBase][ServerUseSkillAtSlot] SkillManagerComponent is null."));
		return;
	}

	SkillManagerComponent->UseSkillAtSlot(SlotIndex); // 서버에서 스킬 사용
}

bool ATFDCharacterBase::ServerUseSkillAtSlot_Validate(int32 SlotIndex)
{
	if (!SkillManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDCharacterBase][ServerUseSkillAtSlot] SkillManagerComponent is null."));
		return false;
	}

	// 슬롯 유효성 검사
	const bool bIsValidIndex = (SlotIndex >= 0 && SlotIndex < SkillManagerComponent->GetMaxSlotCount());
	if (!bIsValidIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDCharacterBase][ServerUseSkillAtSlot] Invalid SlotIndex: %d"), SlotIndex);
	}
	return bIsValidIndex;
}

void ATFDCharacterBase::BaseSetting()
{
	UE_LOG(LogTemp, Warning, TEXT("BaseSetting"));
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void ATFDCharacterBase::SetDAPlayerStat()
{
	if (!HasAuthority() || !CharacterData.Get())
		return;
	// AttributeSet의 초기값을 데이터 에셋의 값으로 설정
	AttributeSet->SetHealth(CharacterData->Health);
	AttributeSet->SetMaxHealth(CharacterData->MaxHealth);
	AttributeSet->SetMana(CharacterData->Mana);
	AttributeSet->SetMaxMana(CharacterData->MaxMana);
	AttributeSet->SetSpeed(CharacterData->Speed);
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UTFDAttributeSet::GetSpeedAttribute()).AddUObject(this, &ATFDCharacterBase::OnSpeedAttributeChanged);
	}

	//JobDataAsset - Give Ability
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}

	//JobDataAsset - 팀태그 넘겨주는 코드
	if (CharacterData->GiveTeamtagEffect)
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			CharacterData->GiveTeamtagEffect,
			1.0f,
			ContextHandle
		);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	//도둑일 경우 골드 습득 시 OnGoldAttributeChanged 연결
	if (AbilitySystemComponent && CharacterData->TeamTag == TAG_Team_Thief)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UTFDAttributeSet::GetGoldAttribute()).AddUObject(this, &ATFDCharacterBase::OnGoldAttributeChanged);
	}
}

void ATFDCharacterBase::OnSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("[ATFDCharacterBase][Speed Changed] Old: %f → New: %f (Character: %s)"), Data.OldValue, Data.NewValue, *GetName());

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Data.NewValue;
		UE_LOG(LogTemp, Warning, TEXT("[ATFDCharacterBase][Speed Changed] MaxWalkSpeed updated to: %f"), MoveComp->MaxWalkSpeed);
	}
}

void ATFDCharacterBase::OnGoldAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (HasAuthority())
	{
		ATFDGameState* GS = Cast<ATFDGameState>(GetWorld()->GetGameState());
		if (!GS) return;

		float AddScore = Data.NewValue - Data.OldValue;
		GS->AddThiefScore(AddScore);
	}
}
