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
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerController_Title][JoinServer] ServerIP is empty!"));
		return;
	}

	// "ip:port" 형식일 수도 있으니 그냥 그대로 씀
	FString TravelURL = ServerIP;

	// 만약 포트를 지정 안했다면 기본 포트 7777 사용하도록 하려면 아래처럼 설정 가능
	if (!ServerIP.Contains(TEXT(":")))
	{
		TravelURL = FString::Printf(TEXT("%s:7777"), *ServerIP);
	}

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
