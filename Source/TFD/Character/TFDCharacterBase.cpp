#include "Character/TFDCharacterBase.h"
#include "GameAbilitySystem/Attibute/TFDAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "TFDPlayerDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ATFDCharacterBase::ATFDCharacterBase()
{

	PrimaryActorTick.bCanEverTick = true;

	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UTFDAttributeSet>(TEXT("AttributeSet"));

}

// Called when the game starts or when spawned
void ATFDCharacterBase::BeginPlay()
{
	Super::BeginPlay();

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

	// 서버일 때만 실행
	if (HasAuthority())
	{

		if (AbilitySystemComponent)
		{
			// ASC의 Owner는 PlayerState, Avatar는 이 캐릭터(Pawn)
			AbilitySystemComponent->InitAbilityActorInfo(this, this);
		}
	}

	SetDAPlayerStat();

}

void ATFDCharacterBase::BaseSetting()
{
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
	if (CharacterData && AttributeSet)
	{
		// AttributeSet의 초기값을 데이터 에셋의 값으로 설정
		AttributeSet->SetHealth(CharacterData->Health);
		AttributeSet->SetMaxHealth(CharacterData->MaxHealth);
		AttributeSet->SetMana(CharacterData->Mana);
		AttributeSet->SetMaxMana(CharacterData->MaxMana);
		AttributeSet->SetSpeed(CharacterData->Speed);

		if (GetCharacterMovement() && AttributeSet)
		{
			GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
		}

		for (const auto& AbilityClass : CharacterData->StartupAbilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
			}
		}
	}
}