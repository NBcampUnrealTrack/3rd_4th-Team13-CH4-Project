// Copyright Epic Games, Inc. All Rights Reserved.

#include "TFDCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameAbilitySystem/Attibute/TFDAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "TFDPlayerDataAsset.h"
#include "PlayerState/TFDPlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATFDCharacter

ATFDCharacter::ATFDCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UTFDAttributeSet>(TEXT("AttributeSet"));
	BaseSetting();
}

TArray<TSubclassOf<UGameplayAbility>> ATFDCharacter::GetStartupAbilities() const
{
	return StartupAbilities;
}

UAbilitySystemComponent* ATFDCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void ATFDCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ATFDCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 서버일 때만 실행
	if (HasAuthority())
	{
		// PlayerState에 접근하여 ASC 초기화
		if (ATFDPlayerState* pPlayerState = Cast<ATFDPlayerState>(GetPlayerState()))
		{
			if (AbilitySystemComponent)
			{
				// ASC의 Owner는 PlayerState, Avatar는 이 캐릭터(Pawn)
				AbilitySystemComponent->InitAbilityActorInfo(pPlayerState, this);
			}
		}
       
		SetDAPlayerStat();
	
		
		for (const auto& AbilityClass : StartupAbilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
			}
		}
	}
}


void ATFDCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFDCharacter::BaseSetting()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATFDCharacter::SetDAPlayerStat()
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
	}
}
