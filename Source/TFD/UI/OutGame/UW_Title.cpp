// UW_Title.cpp
#include "UI/OutGame/UW_Title.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/TFDPlayerController_Title.h"
#include "GameInstance/TFDGameInstance.h"

void UUW_Title::OnCreateServerClicked()
{
	if (ATFDPlayerController_Title* PC = GetOwningPlayer<ATFDPlayerController_Title>())
	{
		PC->CreateServer();
		UTFDGameInstance* TFDGI = Cast<UTFDGameInstance>(GetGameInstance());
		TFDGI->PlayUISound(EUISoundType::Click_00);
	}
}

void UUW_Title::OnJoinServerClicked()
{
	// UI 전환
	if (ATFDPlayerController_Title* PC = GetOwningPlayer<ATFDPlayerController_Title>())
	{
		PC->ShowEnterIPUI();
	}
}

void UUW_Title::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UUW_Title::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_CreateServer)
	{
		Btn_CreateServer->OnClicked.AddDynamic(this, &UUW_Title::OnCreateServerClicked);
	}

	if (Btn_JoinServer)
	{
		Btn_JoinServer->OnClicked.AddDynamic(this, &UUW_Title::OnJoinServerClicked);
	}

	if (Btn_QuitGame)
	{
		Btn_QuitGame->OnClicked.AddDynamic(this, &UUW_Title::OnQuitGameClicked);
	}
}

void UUW_Title::NativeDestruct()
{
	Super::NativeDestruct();

	if (Btn_CreateServer)
	{
		Btn_CreateServer->OnClicked.RemoveDynamic(this, &UUW_Title::OnCreateServerClicked);
	}

	if (Btn_JoinServer)
	{
		Btn_JoinServer->OnClicked.RemoveDynamic(this, &UUW_Title::OnJoinServerClicked);
	}

	if (Btn_QuitGame)
	{
		Btn_QuitGame->OnClicked.RemoveDynamic(this, &UUW_Title::OnQuitGameClicked);
	}
}
