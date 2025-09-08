// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TFDPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/TFDCharacter.h"
#include "GameFramework/Character.h"

ATFDPlayerController::ATFDPlayerController()
{
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

void ATFDPlayerController::Attack(const FInputActionValue& Value)
{
}

void ATFDPlayerController::TogglePause(const FInputActionValue& Value)
{
}
