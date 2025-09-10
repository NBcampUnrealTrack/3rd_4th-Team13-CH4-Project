// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TFDPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/TFDCharacter.h"
#include "GameFramework/Character.h"
#include "Character/TFDCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "TFDNativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

ATFDPlayerController::ATFDPlayerController()
{
}

void ATFDPlayerController::SetMovemnetWalking()
{
	if (ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement None22"));

		CB->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		CB->GetCharacterMovement()->Activate();
	}
}

void ATFDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		check(DefaultMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);

	}
}

void ATFDPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// if (GameStateListenerHandle.IsValid())
	// {
	// 	UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(GameStateListenerHandle);
	// }
	Super::EndPlay(EndPlayReason);
}

void ATFDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFDPlayerController::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATFDPlayerController::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATFDPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &ATFDPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this,
								   &ATFDPlayerController::Dash);
	}
}

void ATFDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(InPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement None22"));

		CB->GetCharacterMovement()->DisableMovement();
		CB->GetCharacterMovement()->SetMovementMode(MOVE_None);
		CB->GetCharacterMovement()->StopMovementImmediately();
	}
}

void ATFDPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);

	if (IsLocalPlayerController())
	{
		if (ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(InPawn))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			{
				if (CB->CharacterData->JobMappingContext)
				{

					Subsystem->AddMappingContext(CB->CharacterData->JobMappingContext, 0);
				}
			}

			//********직업에 따른 능력 입력 바인딩************
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
			{
				for (auto Action : CB->CharacterData->Actions)
				{
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ATFDPlayerController::JobAbility, Action.Tag);
				}
				
			}
		}
	}
}

void ATFDPlayerController::Dash(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
	
		if (ATFDCharacter* MyCharacter = Cast<ATFDCharacter>(ControlledPawn))
		{
			
			if (UAbilitySystemComponent* ASC = MyCharacter->GetAbilitySystemComponent())
			{
				// StartupAbilities 배열 중 Dash Ability 찾기
			
				for (TSubclassOf<UGameplayAbility> AbilityClass : MyCharacter->GetStartupAbilities())
				{
					if (AbilityClass && AbilityClass->GetName().Contains(TEXT("BP_GA_TFDDashAbility"))) // 이름 기준 예시
					{
						ASC->TryActivateAbilityByClass(AbilityClass);
						break; // 한 번만 발동
					}
				}
			}
		}
	}
}

void ATFDPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InMoveVector = Value.Get<FVector2D>();

		const FRotator ControlrRotation = GetControlRotation();
		const FRotator ControlYawRotation(0.f, ControlrRotation.Yaw, 0.f);

		const FVector InLookVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
		const FVector InRightVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);


		ControlledPawn->AddMovementInput(InLookVector, InMoveVector.X);
		ControlledPawn->AddMovementInput(InRightVector, InMoveVector.Y);
		
	}
}

void ATFDPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InLookVector = Value.Get<FVector2D>();

	AddYawInput(InLookVector.X);
	AddPitchInput(InLookVector.Y);
}

void ATFDPlayerController::Jump(const FInputActionValue& Value)
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->Jump();
	}
}

void ATFDPlayerController::StopJumping()
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->StopJumping();
	}
}


//DataAsset에서 추가한 액션과 태그로 자동 바인딩
void ATFDPlayerController::JobAbility(const FInputActionValue& Value, FGameplayTag InputTag)
{
	ATFDCharacterBase* OwnerCharacter = Cast<ATFDCharacterBase>(GetPawn());
	if (OwnerCharacter)
	{
		UAbilitySystemComponent* AbilitySystemComponent = OwnerCharacter->GetAbilitySystemComponent();
		if (AbilitySystemComponent)
		{
			FGameplayTagContainer AbilityTags;
			AbilityTags.AddTag(InputTag);
			AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
		}
	}
}

/*
void ATFDPlayerController::Attack(const FInputActionValue& Value)
{
}

void ATFDPlayerController::TogglePause(const FInputActionValue& Value)
{
}
*/

