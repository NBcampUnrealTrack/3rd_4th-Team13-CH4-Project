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

void ATFDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetMovemnetWalking(false);
}



void ATFDPlayerController::OnUnPossess()
{
	if (IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Warning, TEXT("UnBind : Job IMC, Job InputAction (Client)"));

		//Job InputAction 초기화
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			for (int32 Handle : JobBindingHandles)
			{
				EnhancedInputComponent->RemoveBindingByHandle(Handle);
			}

			JobBindingHandles.Reset();
		}

		//Job IMC 초기화
		if (ActiveJobIMC.IsValid())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(ActiveJobIMC.Get());
			}

			ActiveJobIMC = nullptr;
		}
	}

	Super::OnUnPossess();
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

	Server_NotifyPlayerIsReady();
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

void ATFDPlayerController::Server_NotifyPlayerIsReady_Implementation()
{
	// 서버에서 실행되는 로직
	if (ATFDGameMode* TFDGameMode = GetWorld()->GetAuthGameMode<ATFDGameMode>())
	{
		TFDGameMode->PlayerIsReady(this);
	}
}

//===================================================
// 이하 OutGame 관련 - Lobby
//===================================================
bool ATFDPlayerController::IsHostPlayer() const
{
	return IsLocalController() && HasAuthority();
}

void ATFDPlayerController::LeaveLobby()
{
	// 클라이언트는 타이틀로 돌아감 (절대 경로로 ClientTravel)
	ClientTravel(TFDGameConstants::TitleLevel, ETravelType::TRAVEL_Absolute);
}

void ATFDPlayerController::StartGame()
{
	if (!IsLocalController() || !HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController][StartGame] Not authorized to start the game"));
		return;
	}

	// UI 제거
	RemoveLobbyUI();

	FString TravelCommand = FString::Printf(TEXT("%s?listen"), TFDGameConstants::GameLevel);
	UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerController][StartGame] ServerTravel to: %s"), *TravelCommand);

	GetWorld()->ServerTravel(TravelCommand, true); // true: seamless travel
}

void ATFDPlayerController::RemoveLobbyUI()
{
	if (LobbyWidgetInstance)
	{
		LobbyWidgetInstance->RemoveFromParent();
		LobbyWidgetInstance = nullptr;
	}
}

FString ATFDPlayerController::GetLocalIP() const
{// 내 컴퓨터의 로컬 IP 주소(127.0.0.1 제외)를 문자열로 가져오는 함수

	// Winsock(윈속)이라는 시스템 API를 초기화
	WSADATA wsaData;

	// WSAStartup : 네트워크 기능 사용 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{// 윈도우에서 소켓 API를 쓰기 위한 초기화
	// 실패 시 기본값 반환
		return TEXT("0.0.0.0");
	}

	// 내 컴퓨터의 이름(호스트네임)을 얻음
	// "내가 누군지"를 알아야 내 IP 주소를 알 수 있음
	// gethostname : 내 컴퓨터 이름 알아내기
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		WSACleanup(); // Winsock(윈속)이라는 시스템 API를 초기화
		return TEXT("0.0.0.0");
	}

	// 호스트 이름으로 IP 주소 목록 얻기
	// (여러 개일 수도 있음 -> 그래서 리스트처럼 받아옴)
	// getaddrinfo : 도메인 이름으로 IP 목록 얻기
	struct addrinfo hints = { 0 };
	hints.ai_family = AF_INET; // IPv4 주소만 (IPv6은 제외)

	struct addrinfo* info = nullptr;
	if (getaddrinfo(hostname, nullptr, &hints, &info) != 0)
	{
		WSACleanup();
		return TEXT("0.0.0.0");
	}

	// 얻은 주소들 중 루프백(127.x.x.x)은 제외하고 실제 IP만 가져오기
	FString LocalIP = TEXT("0.0.0.0");

	for (struct addrinfo* ptr = info; ptr != nullptr; ptr = ptr->ai_next)
	{// 여러 IP가 있을 수 있어서 반복하면서 검사

		sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;
		char ipStr[INET_ADDRSTRLEN] = { 0 };

		// inet_ntop : 바이너리 IP를 문자열로 변환
		inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);

		FString CandidateIP = FString(ipStr);

		// 루프백 주소 거르기
		// 127.x.x.x는 자기 자신(localhost)이므로 제외
		if (!CandidateIP.StartsWith(TEXT("127.")))
		{// 첫 번째로 나오는 "정상적인 IP"를 리턴
			LocalIP = CandidateIP;
			break;
		}
	}

	// 마무리 정리 (메모리 해제 & 소켓 정리)
	freeaddrinfo(info);
	WSACleanup();

	return LocalIP;
}
