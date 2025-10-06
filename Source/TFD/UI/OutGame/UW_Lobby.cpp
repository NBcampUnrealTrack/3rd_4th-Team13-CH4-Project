// UW_Lobby.cpp
#include "UI/OutGame/UW_Lobby.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState/TFDPlayerState.h"
#include "Engine/Player.h"
#include "Engine/World.h"
#include "TFDNativeGameplayTags.h"

#include "Controller/TFDPlayerController.h"
#include "GameState/TFDGameStateBase_Lobby.h"

void UUW_Lobby::UpdateUIByRole()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		ATFDPlayerController* LobbyPC = Cast<ATFDPlayerController>(PC);

		if (LobbyPC && LobbyPC->IsHostPlayer())
		{// Host 전용 UI 보이기
			if (Btn_Play)
			{
				Btn_Play->SetVisibility(ESlateVisibility::Visible);
			}

			if (Txt_HostIP)
			{
				Txt_HostIP->SetVisibility(ESlateVisibility::Visible);
				Txt_HostIP->SetText(FText::FromString(HostIP));
			}
		}
		else
		{// 일반 유저는 감추기
			if (Btn_Play)
			{
				Btn_Play->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (Txt_HostIP)
			{
				Txt_HostIP->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UUW_Lobby::InitHostIP()
{
	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		if (PC->HasAuthority())
		{// Host일 때만 IP 표시

			// 로컬 IP 가져오기
			FString LocalIP = PC->GetLocalIP();

			// 공인 IP 가져오기
			FString PublicIP = PC->GetPublicIP();

			if (LocalIP.IsEmpty())
			{
				LocalIP = TEXT("Unavailable");
			}
			if (PublicIP.IsEmpty())
			{
				PublicIP = TEXT("Fetching..."); // 요청 전이거나 실패 시
			}

			// 두 IP를 모두 표시
			HostIP = FString::Printf(TEXT("Local IP: %s\nPublic IP: %s"), *LocalIP, *PublicIP);

			if (Txt_HostIP)
			{
				Txt_HostIP->SetText(FText::FromString(HostIP));
			}
		}
	}
}

void UUW_Lobby::OnPlayClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UW_Lobby][OnPlayClicked] Host clicked Play"));

	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		PC->StartGame();
	}
}

void UUW_Lobby::OnLeaveClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UW_Lobby][OnLeaveClicked] Player clicked Leave"));

	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		PC->LeaveLobby();
	}
}

void UUW_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (Btn_Leave)
		Btn_Leave->OnClicked.AddDynamic(this, &UUW_Lobby::OnLeaveClicked);

	if (Btn_Play)
		Btn_Play->OnClicked.AddDynamic(this, &UUW_Lobby::OnPlayClicked);

	// IP 델리게이트
	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		if (PC->IsHostPlayer())
		{
			PC->OnPublicIPReady.AddDynamic(this, &UUW_Lobby::OnPublicIPReceived);
		}
	}

	InitHostIP();
	UpdateUIByRole();

	// GameState 델리게이트
	if (ATFDGameStateBase_Lobby* GS = GetWorld()->GetGameState<ATFDGameStateBase_Lobby>())
	{
		GS->OnPlayerListChanged.AddDynamic(this, &UUW_Lobby::HandlePlayerListChanged);

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
			{
				TFDPS->OnPlayerNameChanged.AddDynamic(this, &UUW_Lobby::HandlePlayerListChanged);
			}
		}

		HandlePlayerListChanged();
	}

	//  Timer 설정 (괄호 닫기 포함)
	GetWorld()->GetTimerManager().SetTimer(
		RefreshTimerHandle,
		this,
		&UUW_Lobby::HandlePlayerListChanged,
		1.0f, // 반복 간격
		true, // 반복 여부
		0.5f  // 최초 호출 지연
	);
}

void UUW_Lobby::NativeDestruct()
{
	Super::NativeDestruct();

	if (Btn_Leave)
	{
		Btn_Leave->OnClicked.RemoveDynamic(this, &UUW_Lobby::OnLeaveClicked);
	}

	if (Btn_Play)
	{
		Btn_Play->OnClicked.RemoveDynamic(this, &UUW_Lobby::OnPlayClicked);
	}

	// GameState 델리게이트 언바인딩
	if (ATFDGameStateBase_Lobby* GS = GetWorld()->GetGameState<ATFDGameStateBase_Lobby>())
	{
		GS->OnPlayerListChanged.RemoveDynamic(this, &UUW_Lobby::HandlePlayerListChanged);

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS))
			{
				TFDPS->OnPlayerNameChanged.RemoveDynamic(this, &UUW_Lobby::HandlePlayerListChanged);
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);

}

void UUW_Lobby::OnPublicIPReceived(const FString& PublicIP)
{
	FString LocalIP = TEXT("Unavailable");

	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		LocalIP = PC->GetLocalIP();
		if (LocalIP.IsEmpty())
		{
			LocalIP = TEXT("Unavailable");
		}
	}

	FString PublicText = (PublicIP == TEXT("Unavailable")) ? TEXT("Not Available") : *PublicIP;

	HostIP = FString::Printf(TEXT("Local IP: %s\nPublic IP: %s"), *LocalIP, *PublicText);

	if (Txt_HostIP)
	{
		Txt_HostIP->SetText(FText::FromString(HostIP));
	}
}

void UUW_Lobby::UpdatePlayerList(const TArray<APlayerState*>& PlayerStates)
{
	/*
		PlayerListItem 위젯 클래스를 만들어서 사용
		미리 만들어놓은 위젯이 아니라, 런타임에 동적으로 생성
		스크롤박스 안에 텍스트 블럭들이 동적으로 생성돼서 차곡차곡 들어가는 구조

		예를 들어, 접속자가 3명이라면 아래와 같은 구조
		UW_Lobby (UserWidget)
		└── SB_PlayerList (ScrollBox)
				├── UTextBlock ("PlayerA")
				├── UTextBlock ("PlayerB")
				└── UTextBlock ("PlayerC")
	*/

	if (!SB_PlayerList) return;

	// 기존 리스트 클리어
	SB_PlayerList->ClearChildren();

	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState<AGameStateBase>();
	if (!GS) return;

	// PlayerArray를 복사해서 정렬
	TArray<APlayerState*> SortedPlayerArray = GS->PlayerArray;

	// PlayerId 기준 오름차순 정렬
	SortedPlayerArray.Sort([](const APlayerState& A, const APlayerState& B)
		{
			return A.GetPlayerId() < B.GetPlayerId();
		});

	// 각 플레이어 상태마다 위젯을 생성해서 추가 (접속자 한명 한명의 UI)
	for (APlayerState* PS : SortedPlayerArray)
	{
		ATFDPlayerState* TFDPS = Cast<ATFDPlayerState>(PS);
		if (!TFDPS) continue;

		FString Nickname = TFDPS->GetPlayerName(); // 또는 TFDPS->GetNickname();
		if (Nickname.IsEmpty()) continue;

		FString TeamName;
		if (TFDPS->GetPreferredTeam() == TAG_Team_Cop)
			TeamName = TEXT("Cop");
		else if (TFDPS->GetPreferredTeam() == TAG_Team_Thief)
			TeamName = TEXT("Thief");

		FString DisplayText = TeamName.IsEmpty() ? Nickname : FString::Printf(TEXT("%s (%s)"), *Nickname, *TeamName);

		UTextBlock* PlayerNameText = NewObject<UTextBlock>(this);
		if (PlayerNameText)
		{
			PlayerNameText->SetText(FText::FromString(DisplayText));
			SB_PlayerList->AddChild(PlayerNameText);
		}
	}


}

void UUW_Lobby::HandlePlayerListChanged()
{
	/*
		나중에 플레이어가 추가될 때 사운드를 재생 또는 플레이어 수 제한 확인 등을 하게 된다면
		이 함수에서 처리.
	*/
	if (const AGameStateBase* GS = GetWorld()->GetGameState())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UUW_Lobby][HandlePlayerListChanged] Player Count: %d"), GS->PlayerArray.Num());

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (PS)
			{
				UE_LOG(LogTemp, Warning, TEXT("[UUW_Lobby][HandlePlayerListChanged] Player in list: %s"), *PS->GetPlayerName());
			}
		}

		UpdatePlayerList(GS->PlayerArray);
	}
}