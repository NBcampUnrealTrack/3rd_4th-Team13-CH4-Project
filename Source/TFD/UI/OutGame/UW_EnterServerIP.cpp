// UW_EnterServerIP.cpp
#include "UI/OutGame/UW_EnterServerIP.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/TFDPlayerController_Title.h"
#include "GameInstance/TFDGameInstance.h"

void UUW_EnterServerIP::OnConnectClicked()
{
	if (!EditIP) return;

	const FString EnteredIP = EditIP->GetText().ToString();
	if (EnteredIP.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[UW_EnterServerIP][OnConnectClicked] IP 입력이 비어 있습니다."));
		return;
	}

	if (ATFDPlayerController_Title* PC = GetOwningPlayer<ATFDPlayerController_Title>())
	{
		// UTFDGameInstance* TFDGI = Cast<UTFDGameInstance>(GetGameInstance());
		// if (IsValid(TFDGI))
		// {
		// 	TFDGI->PlayUISound(EUISoundType::Click_00);
		// }
		
		PC->JoinServer(EnteredIP);
		
	}
}

void UUW_EnterServerIP::OnBackClicked()
{
	// UI 전환
	if (ATFDPlayerController_Title* PC = GetOwningPlayer<ATFDPlayerController_Title>())
	{
		PC->ShowTitleUI();
	}
}

void UUW_EnterServerIP::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 IP 값 설정
	if (EditIP)
	{
		EditIP->SetText(FText::FromString(TEXT("127.0.0.1")));
		EditIP->OnTextChanged.AddDynamic(this, &UUW_EnterServerIP::OnEditIPTextChanged);
	}

	if (Btn_ConnectServer)
	{
		Btn_ConnectServer->OnClicked.AddDynamic(this, &UUW_EnterServerIP::OnConnectClicked);
	}

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UUW_EnterServerIP::OnBackClicked);
	}
}

void UUW_EnterServerIP::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditIP)
	{
		EditIP->OnTextChanged.RemoveDynamic(this, &UUW_EnterServerIP::OnEditIPTextChanged);
	}

	if (Btn_ConnectServer)
	{
		Btn_ConnectServer->OnClicked.RemoveDynamic(this, &UUW_EnterServerIP::OnConnectClicked);
	}

	if (Btn_Back)
	{
		Btn_Back->OnClicked.RemoveDynamic(this, &UUW_EnterServerIP::OnBackClicked);
	}
}

void UUW_EnterServerIP::OnEditIPTextChanged(const FText& Text)
{
	if (bIsFirstEdit)
	{
		bIsFirstEdit = false;

		if (EditIP)
		{
			// 무조건 텍스트 비우기
			EditIP->SetText(FText::GetEmpty());
		}
	}
}
