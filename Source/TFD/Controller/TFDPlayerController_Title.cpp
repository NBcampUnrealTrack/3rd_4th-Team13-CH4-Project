// TFDPlayerController_Title.cpp
#include "Controller/TFDPlayerController_Title.h"

#include "Kismet/GameplayStatics.h"
#include "Constants/TFDGameConstants.h"

void ATFDPlayerController_Title::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	bShowMouseCursor = true;

	ShowTitleUI();
}

void ATFDPlayerController_Title::CreateServer()
{
	UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerController_Title][CreateServer] called"));

	// 리슨 서버 시작 + 로비 레벨로 전환
	UGameplayStatics::OpenLevel(GetWorld(), FName(TFDGameConstants::LobbyLevel), true, "listen");
}

void ATFDPlayerController_Title::JoinServer(const FString& ServerIP)
{
	UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerController_Title][JoinServer] called with IP: %s"), *ServerIP);

	if (ServerIP.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[ATFDPlayerController_Title][JoinServer] ServerIP is empty!"));
		return;
	}

	// "ip:port" 형식일 수도 있으니 그냥 그대로 씀
	FString TravelURL = ServerIP;

	/*
		사설 IP 주소 대역 (Private IP Ranges)
		- 네트워크 대역1 IP: 10.0.0.0/8		| 범위: 10.0.0.0 ~ 10.255.255.255
		- 네트워크 대역2 IP: 172.16.0.0/12	| 범위: 172.16.0.0 ~ 172.31.255.255
		- 네트워크 대역3 IP: 192.168.0.0/16	| 범위: 192.168.0.0 ~ 192.168.255.255

		이 외에도 루프백 주소(127.0.0.1, localhost)는 로컬로 간주
	*/

	// 만약 포트를 명시되지 않았다면, IP 주소에 따라 포트를 자동으로 붙임
	if (!ServerIP.Contains(TEXT(":")))
	{
		bool bIsLocal = false;

		// 로컬로 간주할 수 있는지 확인
		if (ServerIP.Equals(TEXT("127.0.0.1")) || ServerIP.Equals(TEXT("localhost")))
		{// 루프백 주소인 경우
			bIsLocal = true;
		}
		else if (ServerIP.StartsWith(TEXT("10.")) || ServerIP.StartsWith(TEXT("192.168.")))
		{// 사설 대역1 또는 대역3
			bIsLocal = true;
		}
		else if (ServerIP.StartsWith(TEXT("172.")))
		{// 사설 대역2
			if (ServerIP.Len() > 6)
			{// 최소 길이 체크: "172.xx"가 되는 구조여야 의미 있음

				// 두 번째 옥텟 추출
				// 예: "172.20.x.x" → "20"
				FString SecondOctetStr = ServerIP.Mid(4, 2);

				// 문자열을 정수로 변환
				int32 SecondOctet = FCString::Atoi(*SecondOctetStr);

				// 16 ~ 31 범위면 사설 IP로 간주
				if (SecondOctet >= 16 && SecondOctet <= 31)
				{
					bIsLocal = true;
				}
			}
		}

		if (bIsLocal)
		{// 로컬 또는 사설 네트워크라면 기본 포트 7777 사용
			TravelURL = FString::Printf(TEXT("%s:7777"), *ServerIP);
		}
		else
		{// 그 외는 외부 IP로 간주
			TravelURL = FString::Printf(TEXT("%s:17777"), *ServerIP);
		}
	}

	// 최종 접속 URL (디버깅용)
	UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController_Title][JoinServer] Final TravelURL: %s"), *TravelURL);

	// 클라이언트 트래블로 서버에 접속
	ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
}

void ATFDPlayerController_Title::ShowTitleUI()
{
	if (EnterIPWidgetInstance)
	{
		EnterIPWidgetInstance->RemoveFromParent();
		EnterIPWidgetInstance = nullptr;
	}

	if (TitleWidgetClass)
	{
		if (TitleWidgetInstance)
		{// 기존 타이틀 UI 제거
			TitleWidgetInstance->RemoveFromParent();
			TitleWidgetInstance = nullptr;
		}

		// 위젯 생성
		TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);

		if (TitleWidgetInstance)
		{// 화면에 추가
			TitleWidgetInstance->AddToViewport();
		}
	}
}

void ATFDPlayerController_Title::ShowEnterIPUI()
{
	if (TitleWidgetInstance)
	{
		TitleWidgetInstance->RemoveFromParent();
		TitleWidgetInstance = nullptr;
	}

	if (EnterIPWidgetClass)
	{
		EnterIPWidgetInstance = CreateWidget<UUserWidget>(this, EnterIPWidgetClass);
		if (EnterIPWidgetInstance)
		{
			EnterIPWidgetInstance->AddToViewport();
		}
	}
}

void ATFDPlayerController_Title::ClientNotifyMaxPlayerReached_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("접속 인원이 가득 찼습니다."));

	if (MaxPlayerPopupClass)
	{
		UUserWidget* PopupWidget = CreateWidget<UUserWidget>(this, MaxPlayerPopupClass);
		if (PopupWidget)
		{
			PopupWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController_Title] MaxPlayerPopupClass is not set!"));
	}

	// 일정 시간 뒤 자동 복귀 처리
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			ClientTravel(TFDGameConstants::TitleLevel, ETravelType::TRAVEL_Absolute);
		}, 3.0f, false); // 3초 후 이동
}
