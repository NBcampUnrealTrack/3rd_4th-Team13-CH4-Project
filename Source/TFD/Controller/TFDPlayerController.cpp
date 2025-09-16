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
#include "GameMode/TFDGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"

// 이하 OutGame 관련 - Lobby
#include "Constants/TFDGameConstants.h"
#include "TimerManager.h"	// 타이머 매니저 관련
#include "Engine/World.h"	// GetWorld() 사용을 위해

// GetLocalIP() 구현을 위함
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include "Windows/HideWindowsPlatformTypes.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"

ATFDPlayerController::ATFDPlayerController()
{
}

void ATFDPlayerController::SetMovemnetWalking(bool bMovement)
{

	if (ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement Waling"));
		if (bMovement)
		{
			UE_LOG(LogTemp, Warning, TEXT("Movement Walking"));

			CB->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			CB->GetCharacterMovement()->Activate();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Movement None"));

			CB->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			CB->GetCharacterMovement()->Activate();
			CB->GetCharacterMovement()->DisableMovement();
			CB->GetCharacterMovement()->SetMovementMode(MOVE_None);
			CB->GetCharacterMovement()->StopMovementImmediately();
		}
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

	//===================================================
	// 이하 OutGame 관련 - Lobby
	//===================================================
	if (IsLocalController() == false)
	{
		return;
	}

	if (IsHostPlayer())
	{
		RequestPublicIP();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		// 현재 로드된 레벨 이름 가져오기
		FString CurrentLevelName = World->GetMapName();
		CurrentLevelName.RemoveFromStart(World->StreamingLevelsPrefix); // 접두어 제거

		// "L_Lobby" 문자열만 추출
		FString LobbyLevelName = FPaths::GetBaseFilename(TFDGameConstants::LobbyLevel);

		if (!CurrentLevelName.Equals(LobbyLevelName))
		{// 로비가 아닐 경우
			bShowMouseCursor = false;
			RemoveLobbyUI();
		}
		else
		{// 로비인 경우
			// 로비에서만 마우스 커서 보이게 설정
			bShowMouseCursor = true;

			if (LobbyWidgetClass && LobbyWidgetInstance == nullptr)
			{
				LobbyWidgetInstance = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
				if (LobbyWidgetInstance)
				{
					LobbyWidgetInstance->AddToViewport();
				}
			}
		}
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
// 팀 희망 선택 서버 RPC
// 클라이언트에서 호출하여 서버에 팀 태그를 전달
void ATFDPlayerController::ServerSetPreferredTeam_Implementation(const FGameplayTag& TeamTag)
{
	if (ATFDPlayerState* PS = GetPlayerState<ATFDPlayerState>())
	{
		PS->SetPreferredTeam(TeamTag);
	}
}
// 팀 희망 선택 서버 RPC 유효성 검사
bool ATFDPlayerController::ServerSetPreferredTeam_Validate(const FGameplayTag& TeamTag)
{
	// 유효성 검사 로직 구현 가능, 지금은 간단히 true 반환
	return true;
}

// 팀 희망 선택 클라이언트 RPC
// 서버에서 호출하여 클라이언트에게 팀 태그를 전달
void ATFDPlayerController::SendPreferredTeam(FGameplayTag TeamTag)
{
	ServerSetPreferredTeam(TeamTag);
}