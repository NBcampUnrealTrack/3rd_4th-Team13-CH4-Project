// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TFDPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/TFDCharacter.h"
#include "GameFramework/Character.h"
#include "Character/TFDCharacterBase.h"
#include "Character/TFDPlayerCharacter.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "TFDNativeGameplayTags.h"
#include "GameMode/TFDGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/TFDPlayerState.h"
#include "GameFramework/PlayerState.h"


#include "UI/GameUIRouterSubsystem.h"
#include "UI/InGame/PlayingWidget.h"
#include "UI/Widget/UHUDLayoutWidget.h"
#include "UI/InGame/ResultWidget.h"
#include "UI/InGame/MiniMapWidget.h"
#include "UI/InGame/ReleaseWidget.h"

#include "Object/JailCell.h"

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
		if (bMovement)
		{
			UE_LOG(LogTemp, Warning, TEXT("Movement Walking"));

			CB->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			CB->GetCharacterMovement()->Activate();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Movement None"));

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

	if (IsLocalController())
	{
		FString CurrentLevelName = FPaths::GetBaseFilename(UGameplayStatics::GetCurrentLevelName(this, true));
		FString LobbyLevelName = FPaths::GetBaseFilename(TFDGameConstants::LobbyLevel); // or TFDGameConstants::LobbyLevel

		if (CurrentLevelName.Equals(LobbyLevelName, ESearchCase::IgnoreCase))
		{
			EnterLobby(); // 처음 로비 레벨에서 시작했을 때만 실행
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
	if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
	{
		// 기존 바인딩 제거 후 다시 바인딩 (중복 방지)
		GS->OnMachInProgress.RemoveDynamic(this, &ATFDPlayerController::HandleMatchInProgress);
		GS->OnMatchWaitingPostMatch.RemoveDynamic(this, &ATFDPlayerController::HandleMatchWaitingPostMatch);

		// 컨트롤러 함수 바인딩
		GS->OnMachInProgress.AddDynamic(this, &ATFDPlayerController::HandleMatchInProgress);
		GS->OnMatchWaitingPostMatch.AddDynamic(this, &ATFDPlayerController::HandleMatchWaitingPostMatch);
	}
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
	if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
	{
		GS->OnMachInProgress.RemoveDynamic(this, &ATFDPlayerController::HandleMatchInProgress);
		GS->OnMatchWaitingPostMatch.RemoveDynamic(this, &ATFDPlayerController::HandleMatchWaitingPostMatch);
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
				if (CB->CharacterData)
				{
					if (CB->CharacterData->JobMappingContext)
					{
						Subsystem->AddMappingContext(CB->CharacterData->JobMappingContext, 0);
					}
				}
			}

			//********직업에 따른 능력 입력 바인딩************
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
			{
				if (CB->CharacterData)
				{
					for (auto& Action : CB->CharacterData->Actions)
					{
						EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ATFDPlayerController::JobAbility, Action.Tag);
					}
				}
			}
		}
	}
}

void ATFDPlayerController::TryJobMapping()
{

	ATFDCharacterBase* CB = Cast<ATFDCharacterBase>(GetPawn());
	if (!CB)
	{
		UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] CB is null"));
		return;
	}

	ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>();
	if (!GS) return;
	
	UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController] GS found"));
	
	ATFDPlayerState* PS = Cast<ATFDPlayerState>(CB->GetPlayerState());
	if (!PS)
	{
		FTimerHandle RetryTimerHandle;
		GetWorldTimerManager().SetTimer(RetryTimerHandle, this, &ATFDPlayerController::TryJobMapping, 0.2f, false);
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController] Retry JobMapping"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController] PS found"));
		if (PS->GetTeamTag() == TAG_Team_Thief)
		{
			CB->CharacterData = GS->GetRuleData()->ThiefDataAsset;
		}
		else if (PS->GetTeamTag() == TAG_Team_Cop)
		{
			CB->CharacterData = GS->GetRuleData()->PoliceDataAsset;
		}
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (CB->CharacterData && CB->CharacterData->JobMappingContext)
		{
			Subsystem->AddMappingContext(CB->CharacterData->JobMappingContext, 0);
		}
	}

	//********직업에 따른 능력 입력 바인딩************
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (CB->CharacterData)
		{
			for (auto& Action : CB->CharacterData->Actions)
			{
				EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ATFDPlayerController::JobAbility, Action.Tag);
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

//===================================================
// 이하 OutGame 관련 - Lobby
//===================================================

void ATFDPlayerController::EnterLobby()
{

	if(IsLocalController())
	{
		if (IsHostPlayer())
		{
			RequestPublicIP();
		}

		bShowMouseCursor = true;
		RemoveLobbyUI();

		if (APlayerState* PS = GetPlayerState<APlayerState>())
		{
			if(ATFDPlayerState* State = Cast<ATFDPlayerState>(PS))
			{
				//플레이어의 팀 태그 초기화
				State->SetTeamTag(FGameplayTag::EmptyTag);
			}
		}

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

void ATFDPlayerController::RequestPublicIP()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.ipify.org"));
	Request->SetVerb("GET");

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (bSuccess && Resp.IsValid())
			{
				CachedPublicIP = Resp->GetContentAsString().TrimStartAndEnd();
				UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerController][RequestPublicIP] 공인 IP 주소: %s"), *CachedPublicIP);

				// Delegate 호출
				OnPublicIPReady.Broadcast(CachedPublicIP);
			}
			else
			{
				CachedPublicIP = TEXT("Unavailable");
				UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController][RequestPublicIP] 공인 IP 가져오기 실패"));

				OnPublicIPReady.Broadcast(CachedPublicIP); // 실패 시에도 브로드캐스트 가능
			}
		});

	Request->ProcessRequest();
}

FString ATFDPlayerController::GetPublicIP() const
{
	return CachedPublicIP.IsEmpty() ? TEXT("Fetching...") : CachedPublicIP;
}

void ATFDPlayerController::HandleMatchInProgress()
{
	bShowMouseCursor = false;

	if (!IsLocalController()) // 서버 전용 PC에서는 UI 건드리지 않음
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] LP no"));
		return;
	}	

	TryJobMapping();

	if (UGameUIRouterSubsystem* UISub = LP->GetSubsystem<UGameUIRouterSubsystem>())
	{
		if (!HUDWidgetClass)
		{
			return;
		}

		UISub->SetHUDLayoutClass(HUDWidgetClass);
		UISub->CreateHUD();

		if (PlayingWidgetClass)
		{
			if (!UISub->PlayingWidget) 
			{
				UISub->PlayingWidget = Cast<UPlayingWidget>(
					UISub->AddWidgetToLayer(EUILayer::GameLayer, PlayingWidgetClass)
				);
			}
		}
		if (MiniMapWidgetClass)
		{
			if (!UISub->MiniMapWidget) 
			{
				UISub->MiniMapWidget = Cast<UMiniMapWidget>(
					UISub->AddWidgetToLayer(EUILayer::GameLayer, MiniMapWidgetClass)
				);
				UISub->MiniMapWidget->SetOwnerPawn(GetPawn());
			}
		}
	}
}

// 게임 종료/포스트 매치 이벤트 발생 시 처리
void ATFDPlayerController::HandleMatchWaitingPostMatch(FGameplayTag WinTeamTag, EGameCompleteType CompleteType)
{
	bShowMouseCursor = true;

	if (!IsLocalController()) // 서버 전용 PC에서는 UI 건드리지 않음
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	

	if (UGameUIRouterSubsystem* UISub = LP->GetSubsystem<UGameUIRouterSubsystem>())
	{
		if (UISub->PlayingWidget)
		{
			UISub->RemoveWidgetFromLayer(EUILayer::GameLayer, UISub->PlayingWidget);
			UISub->PlayingWidget = nullptr;
		}

		if (UISub->MiniMapWidget)
		{
			UISub->RemoveWidgetFromLayer(EUILayer::GameLayer, UISub->MiniMapWidget);
			UISub->MiniMapWidget = nullptr;
		}


		if (ResultWidgetClass)
		{
			if (UUserWidget* Widget = UISub->AddWidgetToLayer(EUILayer::GameLayer, ResultWidgetClass))
			{
				if (UResultWidget* RW = Cast<UResultWidget>(Widget))
				{
					ATFDPlayerCharacter* MyCharacter = Cast<ATFDPlayerCharacter>(GetPawn());
					RW->InitResult(WinTeamTag, CompleteType, MyCharacter);
					UISub->ResultWidget = RW; // Subsystem에 인스턴스 보관
				}
			}
		}
	}
	HandleRemoveReleaseWidget();
}



void ATFDPlayerController::HandleShowReleaseWidget()
{
	if (!IsLocalController()) // 서버 전용 PC에서는 UI 건드리지 않음
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] LP no"));
		return;
	}

	if (UGameUIRouterSubsystem* UISub = LP->GetSubsystem<UGameUIRouterSubsystem>())
	{
		if (!ReleaseWidgetClass) return;
		
		UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] 클래스 있음"));
		if (UUserWidget* Widget = UISub->AddWidgetToLayer(EUILayer::PopupLayer, ReleaseWidgetClass))
		{
			UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] 위젯 있음"));
			if (UReleaseWidget* RW = Cast<UReleaseWidget>(Widget))
			{
				UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController] 캐스트 됨"));
				ATFDPlayerCharacter* MyCharacter = Cast<ATFDPlayerCharacter>(GetPawn());
				UISub->ReleaseWidget = RW;
			}
		}
	}
}

void ATFDPlayerController::HandleRemoveReleaseWidget()
{
	if (!IsLocalController()) // 서버 전용 PC에서는 UI 건드리지 않음
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	if (UGameUIRouterSubsystem* UISub = LP->GetSubsystem<UGameUIRouterSubsystem>())
	{
		if (UISub->ReleaseWidget)
		{
			UISub->RemoveWidgetFromLayer(EUILayer::PopupLayer, UISub->ReleaseWidget);
			UISub->ReleaseWidget = nullptr;
		}
	}
}